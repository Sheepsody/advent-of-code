pub struct Entry {
    signal: [String; 10],
    output: [String; 4],
}

#[aoc_generator(day08)]
fn parse(input: &str) -> Vec<Entry> {
    input
        .lines()
        .map(|x| {
            let mut parts = x.split("|");

            let signal: [String; 10] = parts
                .next()
                .unwrap()
                .split_whitespace()
                .map(|s| s.to_string())
                .collect::<Vec<String>>()
                .try_into() // Try to convert the Vec<String> into a fixed-size array
                .expect("Signal must have at least 10 lines");

            let output: [String; 4] = parts
                .next()
                .unwrap()
                .split_whitespace()
                .map(|s| s.to_string())
                .collect::<Vec<String>>()
                .try_into() // Try to convert the Vec<String> into a fixed-size array
                .expect("Output must have at least 4 lines");

            Entry { signal, output }
        })
        .collect()
}

#[aoc(day08, part1)]
fn part1(entries: &Vec<Entry>) -> u32 {
    entries
        .iter()
        .map(|e| {
            e.output
                .iter()
                .filter(|x| match x.len() {
                    2 | 4 | 3 | 7 => true,
                    _ => false,
                })
                .count() as u32
        })
        .sum()
}

fn get_count(entry: &Entry) -> u32 {
    // Need one and four for solution
    let one = entry.signal.iter().find(|x| x.len() == 2).unwrap();
    let four = entry.signal.iter().find(|x| x.len() == 4).unwrap();

    entry
        .output
        .iter()
        .map(|x| match x.len() {
            // Unique length integers
            2 => 1,
            3 => 7,
            4 => 4,
            7 => 8,
            // Other
            l => {
                match (
                    l,
                    x.chars().filter(|&c| one.contains(c)).count(),
                    x.chars().filter(|&c: &char| four.contains(c)).count(),
                ) {
                    (5, 1, 3) => 5,
                    (5, 2, 3) => 3,
                    (5, _, 2) => 2,
                    (6, 1, _) => 6,
                    (6, _, 3) => 0,
                    (6, _, 4) => 9,
                    _ => unreachable!(),
                }
            }
        })
        .fold(0, |acc, x| acc * 10 + x)
}

#[aoc(day08, part2)]
fn part2(entries: &Vec<Entry>) -> u32 {
    entries.iter().map(|x| get_count(&x)).sum()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn parsing() {}

    #[test]
    fn sample1() {
        let sample =
            "acedgfb cdfbe gcdfa fbcad dab cefabd cdfgeb eafb cagedb ab | cdfeb fcadb cdfeb cdbaf";
        assert_eq!(part2(&parse(&sample)), 5353);
    }

    #[test]
    fn sample2() {
        let sample = indoc::indoc!(
            r#"be cfbegad cbdgef fgaecd cgeb fdcge agebfd fecdb fabcd edb | fdgacbe cefdb cefbgd gcbe
            edbfga begcd cbg gc gcadebf fbgde acbgfd abcde gfcbed gfec | fcgedb cgb dgebacf gc
            fgaebd cg bdaec gdafb agbcfd gdcbef bgcad gfac gcb cdgabef | cg cg fdcagb cbg
            fbegcd cbd adcefb dageb afcb bc aefdc ecdab fgdeca fcdbega | efabcd cedba gadfec cb
            aecbfdg fbg gf bafeg dbefa fcge gcbea fcaegb dgceab fcbdga | gecf egdcabf bgf bfgea
            fgeab ca afcebg bdacfeg cfaedg gcfdb baec bfadeg bafgc acf | gebdcfa ecba ca fadegcb
            dbcfg fgd bdegcaf fgec aegbdf ecdfab fbedc dacgb gdcebf gf | cefg dcbef fcge gbcadfe
            bdfegc cbegaf gecbf dfcage bdacg ed bedf ced adcbefg gebcd | ed bcgafe cdgba cbgef
            egadfb cdbfeg cegd fecab cgb gbdefca cg fgcdab egfdb bfceg | gbdfcae bgc cg cgb
            gcafb gcf dcaebfg ecagb gf abcdeg gaef cafbge fdbac fegbdc | fgae cfgab fg bagce"#
        );
        assert_eq!(part1(&parse(&sample)), 26);
        assert_eq!(part2(&parse(&sample)), 61229);
    }
}
