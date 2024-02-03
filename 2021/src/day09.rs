type Grid = Vec<Vec<i32>>;

const MOVES: [(i32, i32); 4] = [(0, -1), (0, 1), (-1, 0), (1, 0)];

#[aoc_generator(day09)]
fn parse(input: &str) -> Grid {
    input
        .lines()
        .map(|l| l.chars().map(|c| c.to_digit(10).unwrap() as i32).collect())
        .collect()
}

fn is_greater_than_neighbors(grid: &Vec<Vec<i32>>, x: usize, y: usize, cell: i32) -> bool {
    MOVES.iter().all(|&(dx, dy)| {
        grid.get((x as i32 + dx) as usize)
            .and_then(|row| row.get((y as i32 + dy) as usize))
            .map_or(true, |&c| c > cell)
    })
}

#[aoc(day09, part1)]
fn part1(grid: &Grid) -> i32 {
    (0..grid.len())
        .map(|x| {
            (0..grid[0].len())
                .map(|y| {
                    let cell = grid[x][y];
                    if is_greater_than_neighbors(grid, x, y, cell) {
                        1 + cell
                    } else {
                        0
                    }
                })
                .sum::<i32>()
        })
        .sum()
}

fn fill_bassin(grid: &Grid, visited: &mut Vec<Vec<bool>>, x: usize, y: usize) -> u32 {
    // Use stack to avoid stack overflow
    let mut count = 0;
    let mut stack = vec![(x, y)];

    while let Some((x, y)) = stack.pop() {
        if visited[x][y] {
            continue;
        }
        visited[x][y] = true;
        count += 1;

        MOVES.iter().for_each(|&(dx, dy)| {
            let nx = (x as i32 + dx) as usize;
            let ny = (y as i32 + dy) as usize;
            if let Some(&c) = grid.get(nx).and_then(|row| row.get(ny)) {
                if c < 9 && !visited[nx][ny] {
                    stack.push((nx, ny));
                }
            }
        });
    }

    count
}

#[aoc(day09, part2)]
fn part2(grid: &Grid) -> u32 {
    let mut bassins: Vec<u32> = Vec::new();
    let mut visited = vec![vec![false; grid[0].len()]; grid.len()];

    for x in 0..grid.len() {
        for y in 0..grid[0].len() {
            if grid[x][y] != 9 && !visited[x][y] {
                bassins.push(fill_bassin(&grid, &mut visited, x, y));
            }
        }
    }

    bassins.sort();
    bassins.iter().rev().take(3).fold(1, |a, b| a * b)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn parsing() {
        let sample = indoc::indoc!(
            "12
            34"
        );
        assert_eq!(parse(sample), vec![vec![1, 2], vec![3, 4]]);
    }

    #[test]
    fn sample1() {
        let sample = indoc::indoc!(
            "2199943210
            3987894921
            9856789892
            8767896789
            9899965678"
        );
        assert_eq!(part1(&parse(sample)), 15);
        assert_eq!(part2(&parse(sample)), 1134);
    }
}
