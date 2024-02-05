use std::collections::BTreeMap;
use std::mem;

pub struct Input {
    rules: BTreeMap<(char, char), char>,
    pairs: BTreeMap<(char, char), u64>,
    last: char,
}

#[aoc_generator(day14)]
fn parse(input: &str) -> Input {
    let (spolymer, srules) = input.split_once("\n\n").unwrap();

    let rules = srules.lines().map(|l| l.split_once(" -> ").unwrap()).fold(
        BTreeMap::new(),
        |mut acc, (k, v)| {
            acc.insert(
                (k.chars().nth(0).unwrap(), k.chars().nth(1).unwrap()),
                v.chars().next().unwrap(),
            );
            acc
        },
    );

    let pairs: BTreeMap<(char, char), u64> =
        spolymer
            .chars()
            .collect::<Vec<_>>()
            .windows(2)
            .fold(BTreeMap::new(), |mut acc, p| {
                *acc.entry((p[0], p[1])).or_insert(0) += 1;
                acc
            });

    Input {
        rules,
        pairs,
        last: spolymer.chars().last().unwrap(),
    }
}

fn get_result(input: &Input, nsteps: usize) -> u64 {
    let mut polymer_pairs = input.pairs.clone();

    for _ in 0..nsteps {
        let mut new_pairs = BTreeMap::new();
        for (p, f) in polymer_pairs.iter() {
            if let Some(c) = input.rules.get(p) {
                *new_pairs.entry((p.0, c.clone())).or_insert(0) += f;
                *new_pairs.entry((c.clone(), p.1)).or_insert(0) += f;
            }
        }

        mem::swap(&mut polymer_pairs, &mut new_pairs);
    }

    polymer_pairs.insert((input.last, '$'), 1);

    let (min, max) = polymer_pairs
        .iter()
        .fold(BTreeMap::new(), |mut acc, (p, f)| {
            *acc.entry(p.0).or_insert(0) += f;
            acc
        })
        .iter()
        .fold((u64::MAX, 0), |(min, max), (_, i)| {
            (min.min(*i), max.max(*i))
        });

    max - min
}

#[aoc(day14, part1)]
fn part1(input: &Input) -> u64 {
    get_result(input, 10)
}

#[aoc(day14, part2)]
fn part2(input: &Input) -> u64 {
    get_result(input, 40)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn sample1() {
        let sample = indoc::indoc! {
            "NNCB

            CH -> B
            HH -> N
            CB -> H
            NH -> C
            HB -> C
            HC -> B
            HN -> C
            NN -> C
            BH -> H
            NC -> B
            NB -> B
            BN -> B
            BB -> N
            BC -> B
            CC -> N
            CN -> C"
        };

        assert_eq!(part1(&parse(sample)), 1588);
        assert_eq!(part2(&parse(sample)), 2188189693529);
    }
}
