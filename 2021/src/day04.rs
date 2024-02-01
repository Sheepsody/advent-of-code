type Board = Vec<u32>;

pub struct Bingo {
    numbers: Vec<u32>,
    boards: Vec<Board>,
}

const MASK_ROW: u32 = 0b11111;
const MASK_COL: u32 = 0b100001000010000100001;

#[aoc_generator(day04)]
pub fn input_generator(input: &str) -> Bingo {
    let mut blocks: Vec<&str> = input.split("\n\n").collect();

    let numbers: Vec<u32> = blocks
        .remove(0)
        .split(",")
        .map(|n| n.parse().unwrap())
        .collect();

    let boards: Vec<Board> = blocks
        .iter()
        .map(|b| b.split_whitespace().map(|c| c.parse().unwrap()).collect())
        .collect();

    Bingo { numbers, boards }
}

fn process_board(board: &Board, numbers: &Vec<u32>) -> Option<(usize, u32)> {
    let mut mask = 0;

    numbers.iter().enumerate().find_map(|(i, n)| {
        board.iter().enumerate().find_map(|(j, b)| {
            if n == b {
                mask |= 1 << j;
                let mask_row = MASK_ROW << (5 * (j / 5));
                let mask_col = MASK_COL << (j % 5);
                if ((mask & mask_row) == mask_row) || ((mask & mask_col) == mask_col) {
                    let sum = board
                        .iter()
                        .enumerate()
                        .fold(0, |acc, (k, v)| acc + (1 - ((mask >> k) & 1)) * v);
                    return Some((i, sum));
                }
            }
            None
        })
    })
}

#[aoc(day04, part1)]
pub fn part1(input: &Bingo) -> u32 {
    let s = input
        .boards
        .iter()
        .filter_map(|b| process_board(&b, &input.numbers))
        .min_by_key(|&v| v.0)
        .unwrap();

    s.1 * input.numbers[s.0]
}

#[aoc(day04, part2)]
pub fn part2(input: &Bingo) -> u32 {
    let s = input
        .boards
        .iter()
        .filter_map(|b| process_board(&b, &input.numbers))
        .max_by_key(|&v| v.0)
        .unwrap();

    s.1 * input.numbers[s.0]
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn sample1() {
        let sample = indoc::indoc!(
            "7,4,9,5,11,17,23,2,0,14,21,24,10,16,13,6,15,25,12,22,18,20,8,19,3,26,1

            22 13 17 11  0
             8  2 23  4 24
            21  9 14 16  7
             6 10  3 18  5
             1 12 20 15 19

             3 15  0  2 22
             9 18 13 17  5
            19  8  7 25 23
            20 11 10 24  4
            14 21 16 12  6

            14 21 17 24  4
            10 16 15  9 19
            18  8 23 26 20
            22 11 13  6  5
             2  0 12  3  7"
        );

        assert_eq!(part1(&input_generator(sample)), 4512);
        assert_eq!(part2(&input_generator(sample)), 1924);
    }
}
