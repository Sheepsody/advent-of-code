use std::iter;

pub struct Input {
    algo: Vec<bool>,
    grid: Vec<Vec<bool>>,
}

#[aoc_generator(day20)]
fn parse(input: &str) -> Input {
    let (salgo, sgrid) = input.split_once("\n\n").unwrap();

    let algo = salgo
        .chars()
        .filter_map(|l| match l {
            '.' => Some(false),
            '#' => Some(true),
            _ => None,
        })
        .collect();

    let grid = sgrid
        .lines()
        .map(|l| {
            l.chars()
                .filter_map(|c| match c {
                    '.' => Some(false),
                    '#' => Some(true),
                    _ => None,
                })
                .collect()
        })
        .collect();

    Input { algo, grid }
}

fn get_extended(n: usize, grid: &Vec<Vec<bool>>) -> Vec<Vec<bool>> {
    let mut extended = vec![vec![false; grid[0].len() + 2 * n]; grid.len() + 2 * n];
    for (i, row) in grid.iter().enumerate() {
        for (j, &cell) in row.iter().enumerate() {
            extended[i + n][j + n] = cell;
        }
    }

    extended
}

fn get_result(input: &Input, ntimes: usize) -> usize {
    let mut extended = get_extended(ntimes, &input.grid);
    let (height, width) = (extended.len(), extended[0].len());

    for n in 0..ntimes {
        // Default value for empty cells
        let default = n % 2 == 1 && input.algo[0];

        // Pre-compute for rows
        let filters: Vec<_> = iter::once(&vec![default; width])
            .chain(extended.iter())
            .chain(iter::once(&vec![default; width]))
            .map(|row| {
                iter::once(&default)
                    .chain(row.iter())
                    .chain(std::iter::once(&default))
                    .collect::<Vec<_>>()
                    .windows(3)
                    .map(|w| {
                        w.iter()
                            .fold(0_u16, |acc, &x| (acc << 1) + if *x { 1 } else { 0 })
                    })
                    .collect::<Vec<_>>()
            })
            .collect();

        // Change cells
        for i in 1..=height {
            for j in 1..=width {
                let key =
                    filters[i - 1][j - 1] << 6 | filters[i][j - 1] << 3 | filters[i + 1][j - 1];
                extended[i - 1][j - 1] = input.algo[key as usize];
            }
        }
    }

    extended
        .iter()
        .map(|row| row.iter().filter(|&&x| x).count())
        .sum()
}

#[aoc(day20, part1)]
fn part1(input: &Input) -> usize {
    get_result(input, 2)
}

#[aoc(day20, part2)]
fn part2(input: &Input) -> usize {
    get_result(input, 50)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn sample() {
        let sample = indoc::indoc!(
            "..#.#..#####.#.#.#.###.##.....###.##.#..###.####..#####..#....#..#..##..##
            #..######.###...####..#..#####..##..#.#####...##.#.#..#.##..#.#......#.###
            .######.###.####...#.##.##..#..#..#####.....#.#....###..#.##......#.....#.
            .#..#..##..#...##.######.####.####.#.#...#.......#..#.#.#...####.##.#.....
            .#..#...##.#.##..#...##.#.##..###.#......#.#.......#.#.#.####.###.##...#..
            ...####.#..#..#.##.#....##..#.####....##...##..#...#......#.#.......#.....
            ..##..####..#...#.#.#...##..#.#..###..#####........#..####......#..#

            #..#.
            #....
            ##..#
            ..#..
            ..###"
        );

        assert_eq!(part1(&parse(sample)), 35);
        assert_eq!(part2(&parse(sample)), 3351);
    }
}
