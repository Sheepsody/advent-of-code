pub struct Input {
    points: Vec<(i16, i16)>,
    folds: Vec<(char, i16)>,
}

#[aoc_generator(day13)]
fn parse(input: &str) -> Input {
    let (sgrid, sfolds) = input.split_once("\n\n").unwrap();

    let points = sgrid
        .lines()
        .map(|x| {
            x.split_once(",")
                .map(|c| (c.0.parse::<i16>().unwrap(), c.1.parse::<i16>().unwrap()))
                .unwrap()
        })
        .collect();

    let folds = sfolds
        .lines()
        .map(|l| {
            l.split_once("=")
                .map(|t| (t.0.chars().nth(11).unwrap(), t.1.parse::<i16>().unwrap()))
                .unwrap()
        })
        .collect();

    Input { points, folds }
}

#[aoc(day13, part1)]
fn part1(input: &Input) -> usize {
    let (c, v) = input.folds[0]; // Get first fold
    let mut points = input
        .points
        .iter()
        .filter_map(|&(x, y)| match c {
            'x' if x == v => None,
            'x' if x > v => Some((2 * v - x, y)),
            'y' if y == v => None,
            'y' if y > v => Some((x, 2 * v - y)),
            _ => Some((x, y)),
        })
        .collect::<Vec<_>>();
    points.sort_unstable();
    points.dedup();
    points.len()
}

fn display(points: &Vec<(i16, i16)>) -> String {
    let (maxx, maxy) = points.iter().fold((0, 0), |mut acc, &(x, y)| {
        if x > acc.0 {
            acc.0 = x
        };
        if y > acc.1 {
            acc.1 = y
        };
        acc
    });

    let mut output: Vec<char> = vec!['.'; (maxx as usize + 2) * (maxy as usize + 1) + 1];
    output[0] = '\n'; // For display
    for i in 0..=maxy {
        output[1 + (i * (maxx + 2) + (maxx + 1)) as usize] = '\n';
    }
    for (x, y) in points {
        output[1 + (y * (maxx + 2) + x) as usize] = '#';
    }

    output.into_iter().collect()
}

#[aoc(day13, part2)]
fn part2(input: &Input) -> String {
    let points = input
        .points
        .iter()
        .filter_map(|&(x, y)| {
            input
                .folds
                .iter()
                .try_fold((x, y), |(x, y), &(c, v)| match c {
                    'x' if x == v => None,
                    'x' if x > v => Some((2 * v - x, y)),
                    'y' if y == v => None,
                    'y' if y > v => Some((x, 2 * v - y)),
                    _ => Some((x, y)),
                })
        })
        .collect::<Vec<_>>();

    display(&points)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn sample1() {
        let sample = indoc::indoc! {
            "6,10
            0,14
            9,10
            0,3
            10,4
            4,11
            6,0
            6,12
            4,1
            0,13
            10,12
            3,4
            3,0
            8,4
            1,10
            2,14
            8,10
            9,0

            fold along y=7
            fold along x=5"
        };

        assert_eq!(part1(&parse(sample)), 17);

        let result = indoc::indoc! {
            "#####
            #...#
            #...#
            #...#
            #####
            "
        };

        assert_eq!(&part2(&parse(sample))[1..], result);
    }
}
