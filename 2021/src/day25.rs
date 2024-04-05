#[derive(Clone, Copy, Eq, PartialEq)]
enum Cell {
    East,
    South,
    Empty,
}

// @note Could go faster with Bits
type Grid = Vec<Vec<Cell>>;

#[aoc_generator(day25)]
fn parse(input: &str) -> Grid {
    input
        .lines()
        .map(|l| {
            l.chars()
                .map(|c| match c {
                    '>' => Cell::East,
                    'v' => Cell::South,
                    _ => Cell::Empty,
                })
                .collect()
        })
        .collect()
}

// @note To be optimized...
fn step(map: &mut Vec<Vec<Cell>>, direction: Cell, dx: usize, dy: usize) -> bool {
    let nrows = map.len();
    let ncols = map[0].len();
    let mut changes = Vec::new();

    for (i, row) in map.iter().enumerate() {
        for (j, &c) in row.iter().enumerate() {
            let (ni, nj) = ((i + dx) % nrows, (j + dy) % ncols);
            if c == direction && map[ni][nj] == Cell::Empty {
                changes.push((i, j, ni, nj));
            }
        }
    }

    let changed = changes.len() > 0;
    for (i, j, ni, nj) in changes.into_iter() {
        map[ni][nj] = direction;
        map[i][j] = Cell::Empty;
    }
    changed
}

#[aoc(day25, part1)]
fn part1(grid: &Grid) -> u32 {
    let mut grid = grid.clone();
    (1..)
        .find_map(|i| {
            let east = step(&mut grid, Cell::East, 0, 1);
            let south = step(&mut grid, Cell::South, 1, 0);
            match (east, south) {
                (false, false) => Some(i),
                _ => None,
            }
        })
        .unwrap()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_sample() {
        let sample = indoc::indoc! {
            "v...>>.vv>
            .vv>>.vv..
            >>.>v>...v
            >>v>>.>.v.
            v>v.vv.v..
            >.>>..v...
            .vv..>.>v.
            v.v..>>v.v
            ....v..v.>"
        };

        assert_eq!(part1(&parse(sample)), 58);
    }
}
