use itertools::Itertools;
use std::collections::BTreeMap;
use std::mem;

#[aoc_generator(day21)]
fn parse(input: &str) -> (usize, usize) {
    input
        .lines()
        .map(|l| l.split_once(": ").unwrap().1.parse().unwrap())
        .collect_tuple()
        .unwrap()
}

#[aoc(day21, part1)]
fn part1(&(a, b): &(usize, usize)) -> usize {
    let (mut pos, mut score) = ([a, b], [0, 0]);

    for turn in 0.. {
        let player = turn % 2;

        let dice = turn * 3 + 1;
        let roll = (dice * 3 + 3 + pos[player] - 1) % 10 + 1;

        pos[player] = roll;
        score[player] += roll;

        if score[player] >= 1000 {
            return score[(player + 1) % 2] * (turn + 1) * 3;
        }
    }

    return 0;
}

// Outcomes of throwing 3 quantum dices
const OUTCOMES: [(u8, u8); 7] = [(3, 1), (4, 3), (5, 6), (6, 7), (7, 6), (8, 3), (9, 1)];

// Get the number of wins per step
// The multiverse is a map of (score, position) -> count
fn get_wins_per_step(start: u8) -> Vec<u64> {
    let mut wins = Vec::new();
    let mut multiverse: BTreeMap<(u8, u8), u64> = BTreeMap::new();

    multiverse.insert((0, start), 1);

    while multiverse.len() > 0 {
        let mut next = BTreeMap::new();
        let mut count: u64 = 0;

        multiverse.iter().for_each(|(&(score, pos), v)| {
            OUTCOMES.iter().for_each(|&(roll, outcome)| {
                let next_pos = (pos + roll - 1) % 10 + 1;
                let next_score = score + next_pos;
                let next_count = (outcome as u64) * v;

                if next_score >= 21 {
                    count += next_count;
                } else {
                    *next.entry((next_score, next_pos)).or_insert(0) += next_count;
                }
            });
        });

        wins.push(count);
        mem::swap(&mut multiverse, &mut next);
    }

    wins
}

#[aoc(day21, part2)]
fn part2(&(a, b): &(usize, usize)) -> u64 {
    // TODO Doc
    let mut wins_a = get_wins_per_step(a as u8);
    let mut wins_b = get_wins_per_step(b as u8);

    let (_, ca, _, cb) = wins_a.iter().zip(wins_b.iter()).fold(
        (1, 0, 1, 0),
        |(mut sa, mut ca, mut sb, mut cb), (wa, wb)| {
            // Number multiverse after account winnings
            // Basically no wins => 27 times more universes (3^3)
            sa = sa * 27 - wa;
            // Win number of multiverse equal to Player's B multiverse up to that point
            ca += wa * sb;

            // Same for B
            sb = sb * 27 - wb;
            cb += wb * sa;

            (sa, ca, sb, cb)
        },
    );

    ca.max(cb)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse() {
        let sample = indoc::indoc!(
            "Player 1 starting position: 4
            Player 2 starting position: 8"
        );
        assert_eq!(parse(sample), (4, 8));
        assert_eq!(part1(&parse(sample)), 739785);
        assert_eq!(part2(&parse(sample)), 444356092776315);
    }
}
