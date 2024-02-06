use std::{
    cmp::Ordering,
    collections::{BinaryHeap, HashSet},
};

// Alias for vec vec u8
type Grid = Vec<Vec<i16>>;

const NEXT: [(i16, i16); 4] = [(1, 0), (-1, 0), (0, 1), (0, -1)];

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
struct Cell {
    p: (i16, i16),
    d: i16,
}

impl Ord for Cell {
    fn cmp(&self, other: &Self) -> Ordering {
        other.d.cmp(&self.d)
    }
}

impl PartialOrd for Cell {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        other.d.partial_cmp(&self.d)
    }
}

#[aoc_generator(day15)]
fn parse(input: &str) -> Grid {
    input
        .lines()
        .map(|l| l.bytes().map(|c| (c - b'0') as i16).collect())
        .collect()
}

// @note : benchmarked a* and dijkstra, dijkstra is faster
fn dijkstra(grid: &Grid) -> i16 {
    let (width, height) = (grid.len(), grid[0].len());

    let mut distances = vec![vec![-1; width]; height];
    let mut visited = vec![vec![false; width]; height];
    let mut queue = BinaryHeap::new();

    distances[0][0] = 0;
    queue.push(Cell { p: (0, 0), d: 0 });

    while let Some(Cell { p, d }) = queue.pop() {
        // Node already visited
        if visited[p.0 as usize][p.1 as usize] {
            continue;
        }
        visited[p.0 as usize][p.1 as usize] = true;

        if (p.0, p.1) == (height as i16 - 1, width as i16 - 1) {
            break;
        }

        NEXT.iter()
            .map(|&(dx, dy)| (p.0 + dx, p.1 + dy))
            .filter(|&(nx, ny)| {
                (nx >= 0) && (ny >= 0) && (nx < (height as i16)) && (ny < (width as i16))
            })
            .for_each(|(nx, ny)| {
                let new_d = d + grid[nx as usize][ny as usize];
                let old_d = distances[nx as usize][ny as usize];

                if new_d < old_d || old_d == -1 {
                    distances[nx as usize][ny as usize] = new_d;
                    queue.push(Cell {
                        p: (nx, ny),
                        d: new_d,
                    });
                }
            });
    }

    distances[grid.len() - 1][grid[0].len() - 1]
}

#[aoc(day15, part1)]
fn part1(grid: &Grid) -> i16 {
    dijkstra(grid)
}

#[aoc(day15, part2)]
fn part2(grid: &Grid) -> i16 {
    let (height, width) = (grid.len(), grid[0].len());
    let mut new_grid = vec![vec![0; width * 5]; height * 5];

    (0..5).for_each(|xx| {
        (0..5).for_each(|yy| {
            (0..height).for_each(|x| {
                (0..width).for_each(|y| {
                    new_grid[x + xx * height][y + yy * width] =
                        (grid[x][y] + xx as i16 + yy as i16 - 1) % 9 + 1;
                });
            });
        });
    });

    dijkstra(&new_grid)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn parsing() {
        let sample = indoc::indoc! {
            "12
            34"
        };
        assert_eq!(parse(sample), vec![vec![1, 2], vec![3, 4]]);
    }

    #[test]
    fn sample1() {
        let sample = indoc::indoc! {
            "1163751742
            1381373672
            2136511328
            3694931569
            7463417111
            1319128137
            1359912421
            3125421639
            1293138521
            2311944581"
        };

        assert_eq!(part1(&parse(sample)), 40);
        assert_eq!(part2(&parse(sample)), 315);
    }
}
