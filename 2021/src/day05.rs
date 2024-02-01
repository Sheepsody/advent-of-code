use std::collections::HashMap;

#[derive(Eq, PartialEq, Debug)]
struct Point {
    x: i32,
    y: i32,
}

#[derive(Eq, PartialEq, Debug)]
pub struct Line {
    start: Point,
    end: Point,
}

fn parse_line(line: &str) -> Option<Line> {
    let mut parts = line.split("->");

    let mut begin = parts
        .next()
        .unwrap()
        .trim()
        .split(',')
        .map(|c| c.parse().unwrap());

    let mut end = parts
        .next()
        .unwrap()
        .trim()
        .split(',')
        .map(|c| c.parse().unwrap());

    Some(Line {
        start: Point {
            x: begin.next()?,
            y: begin.next()?,
        },
        end: Point {
            x: end.next()?,
            y: end.next()?,
        },
    })
}

#[aoc_generator(day05)]
pub fn input_generator(input: &str) -> Vec<Line> {
    input.lines().filter_map(|l| parse_line(l)).collect()
}

#[aoc(day05, part1, hashmap)]
pub fn part1_hashmap(lines: &Vec<Line>) -> i32 {
    let mut map: HashMap<(i32, i32), i32> = HashMap::new();

    for line in lines {
        let (min_x, max_x) = (line.start.x.min(line.end.x), line.start.x.max(line.end.x));
        let (min_y, max_y) = (line.start.y.min(line.end.y), line.start.y.max(line.end.y));

        if min_x == max_x {
            for y in min_y..=max_y {
                *map.entry((line.start.x, y)).or_insert(0) += 1;
            }
        } else if min_y == max_y {
            for x in min_x..=max_x {
                *map.entry((x, line.start.y)).or_insert(0) += 1;
            }
        }
    }

    map.values().filter(|&&v| v > 1).count() as i32
}

#[aoc(day05, part2)]
pub fn part2(lines: &Vec<Line>) -> u32 {
    // @note Found that using a grid was faster
    let (max_x, max_y) = lines.iter().fold((0, 0), |(mx, my), line| {
        (
            mx.max(line.start.x).max(line.end.x),
            my.max(line.start.y).max(line.end.y),
        )
    });
    // @note Could go faster with smaller then u8 (needs only 2 bits)
    let mut grid: Vec<Vec<u8>> = vec![vec![0; (max_y + 1) as usize]; (max_x + 1) as usize];

    for line in lines {
        let (min_x, max_x) = (line.start.x.min(line.end.x), line.start.x.max(line.end.x));
        let (min_y, max_y) = (line.start.y.min(line.end.y), line.start.y.max(line.end.y));

        if min_x == max_x {
            for y in min_y..=max_y {
                grid[min_x as usize][y as usize] += 1;
            }
        } else if min_y == max_y {
            for x in min_x..=max_x {
                grid[x as usize][min_y as usize] += 1;
            }
        } else {
            let dx = if line.start.x < line.end.x { 1 } else { -1 };
            let dy = if line.start.y < line.end.y { 1 } else { -1 };
            for i in 0..=(max_x - min_x) {
                let (x, y) = (line.start.x + i * dx, line.start.y + i * dy);
                grid[x as usize][y as usize] += 1;
            }
        }
    }

    grid.iter().flatten().filter(|&&v| v > 1).count() as u32
}

#[aoc(day05, part1)]
pub fn part1(lines: &Vec<Line>) -> u32 {
    let (max_x, max_y) = lines.iter().fold((0, 0), |(mx, my), line| {
        (
            mx.max(line.start.x).max(line.end.x),
            my.max(line.start.y).max(line.end.y),
        )
    });
    let mut grid: Vec<Vec<u8>> = vec![vec![0; (max_y + 1) as usize]; (max_x + 1) as usize];

    for line in lines {
        let (min_x, max_x) = (line.start.x.min(line.end.x), line.start.x.max(line.end.x));
        let (min_y, max_y) = (line.start.y.min(line.end.y), line.start.y.max(line.end.y));

        if min_x == max_x {
            for y in min_y..=max_y {
                grid[min_x as usize][y as usize] += 1;
            }
        } else if min_y == max_y {
            for x in min_x..=max_x {
                grid[x as usize][min_y as usize] += 1;
            }
        }
    }

    grid.iter().flatten().filter(|&&v| v > 1).count() as u32
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn parsing() {
        assert_eq!(
            parse_line("0,9 -> 5,9"),
            Some(Line {
                start: Point { x: 0, y: 9 },
                end: Point { x: 5, y: 9 }
            })
        );
    }

    #[test]
    fn sample1() {
        let sample = indoc::indoc!(
            "0,9 -> 5,9
            8,0 -> 0,8
            9,4 -> 3,4
            2,2 -> 2,1
            7,0 -> 7,4
            6,4 -> 2,0
            0,9 -> 2,9
            3,4 -> 1,4
            0,0 -> 8,8
            5,5 -> 8,2"
        );

        assert_eq!(part1(&input_generator(sample)), 5);
        assert_eq!(part2(&input_generator(sample)), 12);
    }
}
