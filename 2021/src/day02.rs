#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Direction {
    FORWARD,
    DOWN,
    UP,
}

#[aoc_generator(day02)]
pub fn input_generator(input: &str) -> Vec<(Direction, u32)> {
    input
        .lines()
        .map(|line| {
            let mut words = line.split_whitespace();

            let direction = match words.next().unwrap() {
                "forward" => Direction::FORWARD,
                "down" => Direction::DOWN,
                "up" => Direction::UP,
                _ => panic!("Invalid direction"),
            };
            let distance = words.next().unwrap().parse::<u32>().unwrap();

            (direction, distance)
        })
        .collect()
}

#[aoc(day02, part1)]
pub fn part1(input: &[(Direction, u32)]) -> u32 {
    let mut position = (0, 0);
    for (direction, distance) in input {
        match direction {
            Direction::FORWARD => position.0 += distance,
            Direction::DOWN => position.1 += distance,
            Direction::UP => position.1 -= distance,
        };
    }

    position.0 * position.1
}

#[aoc(day02, part2)]
pub fn part2(input: &[(Direction, u32)]) -> u32 {
    let mut position = (0, 0);
    let mut aim = 0;
    for (direction, distance) in input {
        match direction {
            Direction::FORWARD => {
                position.0 += distance;
                position.1 += distance * aim;
            }
            Direction::DOWN => aim += distance,
            Direction::UP => aim -= distance,
        };
    }

    position.0 * position.1
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn sample1() {
        let sample = indoc::indoc!(
            "forward 5
            down 5
            forward 8
            up 3
            down 8
            forward 2"
        );

        assert_eq!(part1(&input_generator(sample)), 150);
        assert_eq!(part2(&input_generator(sample)), 900);
    }
}
