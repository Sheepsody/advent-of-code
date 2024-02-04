type Grid = Vec<Vec<u8>>;

#[aoc_generator(day11)]
fn parse(input: &str) -> Grid {
    input
        .lines()
        .map(|l| l.chars().map(|c| c.to_digit(10).unwrap() as u8).collect())
        .collect()
}

const ADJACENT: [(isize, isize); 8] = [
    (0, -1),
    (1, -1),
    (1, 0),
    (1, 1),
    (0, 1),
    (-1, 1),
    (-1, 0),
    (-1, -1),
];

// @note Faster then benchmarked stack-based version
fn flash(m: &mut Grid, i: usize, j: usize) -> u32 {
    m[i][j] = 0;
    ADJACENT
        .iter()
        .map(|(ii, jj)| ((i as isize + ii) as usize, (j as isize + jj) as usize))
        .fold(1, |acc, (i, j)| {
            match m.get_mut(i).and_then(|row| row.get_mut(j)) {
                Some(n) if *n > 0 => {
                    *n += 1;
                    acc + (*n > 9).then(|| flash(m, i, j)).unwrap_or(0)
                }
                _ => acc,
            }
        })
}

fn step(grid: &mut Grid, height: usize, width: usize) -> u32 {
    // Increment all cells
    grid.iter_mut()
        .for_each(|row| row.iter_mut().for_each(|n| *n += 1));
    // Flash cells
    (0..height)
        .flat_map(|i| (0..width).map(move |j| (i, j)))
        .map(|(i, j)| if grid[i][j] > 9 { flash(grid, i, j) } else { 0 })
        .sum::<u32>()
}

#[aoc(day11, part1)]
fn part1(grid: &Grid) -> u32 {
    let mut grid = grid.clone();
    let height = grid.len();
    let width = grid[0].len();

    (0..100).map(|_| step(&mut grid, height, width)).sum()
}

#[aoc(day11, part2)]
fn part2(grid: &Grid) -> u32 {
    let mut grid = grid.clone();
    let height = grid.len();
    let width = grid[0].len();

    (1..)
        .find(|_| step(&mut grid, height, width) == (width * height) as u32)
        .unwrap()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn sample1() {
        let sample = indoc::indoc!(
            "5483143223
            2745854711
            5264556173
            6141336146
            6357385478
            4167524645
            2176841721
            6882881134
            4846848554
            5283751526"
        );

        assert_eq!(part1(&parse(&sample)), 1656);
        assert_eq!(part2(&parse(&sample)), 195);
    }
}
