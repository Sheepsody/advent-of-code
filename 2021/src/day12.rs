use std::collections::{BTreeMap, BTreeSet};

// @note Big caves have negative values
type Graph = BTreeMap<i8, Vec<i8>>;

#[aoc_generator(day12)]
fn parse(input: &str) -> Graph {
    let mut idx: BTreeMap<&str, i8> = BTreeMap::from([("start", 0), ("end", 1)]);
    let mut graph: Graph = BTreeMap::new();

    let mut vertice = |x| {
        let length = idx.len() as i8;
        idx.entry(x).or_insert_with(|| {
            if x.chars().nth(0).unwrap().is_lowercase() {
                length
            } else {
                -length
            }
        });
        idx[x]
    };

    let mut edge = |a, b| {
        let entry = graph.entry(a).or_insert(vec![]);
        entry.push(b);
    };

    input
        .lines()
        .map(|l| l.split_once('-').unwrap())
        .for_each(|(a, b)| {
            let (a, b) = (vertice(a), vertice(b));
            edge(a, b);
            edge(b, a);
        });

    graph
}

fn count_paths(
    current: i8,
    path: &mut BTreeSet<i8>,
    graph: &Graph,
    mem: &mut BTreeMap<(i8, Vec<i8>), u32>,
) -> u32 {
    graph
        .get(&current)
        .unwrap()
        .iter()
        .map(|next| {
            match next {
                // Start
                0 => 0,
                // End
                1 => 1,
                // Invalid
                next if (next > &1 && path.contains(next)) => 0,
                // Add
                next => {
                    if *next > 0 {
                        path.insert(*next);
                    };
                    let count = count_paths(*next, path, graph, mem);
                    if *next > 0 {
                        path.remove(next);
                    };
                    count
                }
            }
        })
        .sum::<u32>()
}

#[aoc(day12, part1)]
fn part1(graph: &Graph) -> u32 {
    let mut visited = BTreeMap::new();
    let mut path = BTreeSet::new();
    count_paths(0, &mut path, graph, &mut visited)
}

fn count_paths_2(
    current: i8,
    path: &mut BTreeSet<i8>,
    graph: &Graph,
    mem: &mut BTreeMap<(i8, Vec<i8>), u32>,
    double: bool,
) -> u32 {
    graph
        .get(&current)
        .unwrap()
        .iter()
        .map(|next| {
            match next {
                // Start
                0 => 0,
                // End
                1 => 1,
                // Invalid
                next if (next > &1 && path.contains(next) && double) => 0,
                // Add
                next => {
                    if path.contains(next) {
                        count_paths_2(*next, path, graph, mem, true)
                    } else {
                        if *next > 0 {
                            path.insert(*next);
                        };

                        let count = count_paths_2(*next, path, graph, mem, double);
                        if *next > 0 {
                            path.remove(next);
                        };
                        count
                    }
                }
            }
        })
        .sum::<u32>()
}

#[aoc(day12, part2)]
fn part2(graph: &Graph) -> u32 {
    let mut visited = BTreeMap::new();
    let mut path = BTreeSet::new();
    count_paths_2(0, &mut path, graph, &mut visited, false)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn sample1() {
        let sample = indoc::indoc!(
            "start-A
            start-b
            A-c
            A-b
            b-d
            A-end
            b-end
            start-a"
        );
        assert_eq!(part1(&parse(sample)), 10);
        assert_eq!(part2(&parse(sample)), 36);
    }

    #[test]
    fn sample2() {
        let sample = indoc::indoc!(
            "dc-end
            HN-start
            start-kj
            dc-start
            dc-HN
            LN-dc
            HN-end
            kj-sa
            kj-HN
            kj-dc"
        );
        assert_eq!(part1(&parse(sample)), 19);
        assert_eq!(part2(&parse(sample)), 103);
    }

    #[test]
    fn sample3() {
        let sample = indoc::indoc!(
            "fs-end
            he-DX
            fs-he
            start-DX
            pj-DX
            end-zg
            zg-sl
            zg-pj
            pj-he
            RW-he
            fs-DX
            pj-RW
            zg-RW
            start-pj
            he-WI
            zg-he
            pj-fs
            start-RW"
        );
        assert_eq!(part1(&parse(sample)), 226);
        assert_eq!(part2(&parse(sample)), 3509);
    }
}
