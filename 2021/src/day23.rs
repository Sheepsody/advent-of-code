use std::{
    collections::{BinaryHeap, HashMap},
    hash::Hash,
};

use itertools::chain;

#[derive(Debug, Copy, Clone, Eq, PartialEq, Hash)]
enum AmphipodType {
    Amber,
    Bronze,
    Copper,
    Desert,
}

impl AmphipodType {
    fn try_from(c: char) -> Option<Self> {
        match c {
            'A' => Some(Self::Amber),
            'B' => Some(Self::Bronze),
            'C' => Some(Self::Copper),
            'D' => Some(Self::Desert),
            _ => None,
        }
    }
}

#[derive(Debug, Copy, Clone, Eq, PartialEq, Hash)]
struct Amphipod {
    t: AmphipodType,
    x: i8,
    y: i8,
}

impl Amphipod {
    fn energy(&self) -> u32 {
        match self.t {
            AmphipodType::Amber => 1,
            AmphipodType::Bronze => 10,
            AmphipodType::Copper => 100,
            AmphipodType::Desert => 1000,
        }
    }

    fn get_desty(&self) -> i8 {
        match self.t {
            AmphipodType::Amber => 2,
            AmphipodType::Bronze => 4,
            AmphipodType::Copper => 6,
            AmphipodType::Desert => 8,
        }
    }
}

#[derive(Debug, Clone, Eq, PartialEq)]
struct State<const LENGTH: usize> {
    energy: u32,
    amphipods: [Amphipod; LENGTH],
}

impl<const LENGTH: usize> Ord for State<LENGTH> {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        other.energy.cmp(&self.energy)
    }
}

impl<const LENGTH: usize> PartialOrd for State<LENGTH> {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(other.energy.cmp(&self.energy))
    }
}

impl<const LENGTH: usize> State<LENGTH> {
    fn is_goal(&self) -> bool {
        self.amphipods
            .iter()
            .all(|amphi| (amphi.x != 0) && (amphi.y == amphi.get_desty()))
    }

    /*
     * Compress each state into a single integer for memoization
     */
    fn get_code(&self) -> u128 {
        self.amphipods.iter().fold(0, |acc, amphi| {
            (acc << 8) + ((amphi.x as u128) << 4) + (amphi.y as u128)
        })
    }

    fn from_amphipods(amphipods: &Vec<Amphipod>) -> Self {
        State {
            energy: 0,
            amphipods: amphipods.as_slice().try_into().unwrap(),
        }
    }

    fn get_successors(&self) -> Vec<Self> {
        self.to_hallway()
            .into_iter()
            .chain(self.to_room())
            .collect()
    }

    /*
     * Rules
     * 1. Never stop space outside room
     * 2. Stay same spot hallway until can enter room
     */
    fn to_hallway(&self) -> Vec<Self> {
        self.amphipods
            .iter()
            .enumerate()
            .filter(|(_, amphi)| amphi.x != 0)
            .filter(|(i, amphi)| {
                // Either not in position or following not in position
                let mut can_move = self
                    .amphipods
                    .iter()
                    .any(|a| a.y == amphi.y && a.y != a.get_desty());

                // No one blocking the way
                can_move &= self
                    .amphipods
                    .iter()
                    .enumerate()
                    .all(|(j, a)| a.y != amphi.y || a.x > amphi.x || i == &j);

                can_move
            })
            .fold(vec![], |mut acc, (i, amphi)| {
                let miny = self
                    .amphipods
                    .iter()
                    .filter_map(|a| {
                        if a.x == 0 && a.y < amphi.y {
                            Some(a.y)
                        } else {
                            None
                        }
                    })
                    .max()
                    .unwrap_or(-1);
                let maxy = self
                    .amphipods
                    .iter()
                    .filter_map(|a| {
                        if a.x == 0 && a.y > amphi.y {
                            Some(a.y)
                        } else {
                            None
                        }
                    })
                    .min()
                    .unwrap_or(11);

                for y in chain!(miny + 1..=amphi.y, amphi.y..=maxy - 1)
                    .filter(|&x| x != 2 && x != 4 && x != 6 && x != 8)
                {
                    let mut amphipods = self.amphipods.clone();

                    amphipods[i].x = 0;
                    amphipods[i].y = y;

                    let distance = (amphi.y - y).abs() + amphi.x;
                    let energy = self.energy + amphi.energy() * distance as u32;

                    acc.push(State { energy, amphipods });
                }

                acc
            })
    }

    /* Rules
     * 1. never move from the hallway into a room unless that room is their destination
     * 2. That room contains no amphipods which do not also have that room as their destination
     */
    fn to_room(&self) -> Vec<Self> {
        // Can only move amphipods in hallway
        self.amphipods
            .iter()
            .enumerate()
            .filter(|&(_, a)|
                // 1. Must be in hallway
                a.x == 0)
            .filter(|&(_, a)|
                // 2. Room must be only comprised of same element
                !self
                    .amphipods
                    .iter()
                    .any(|&other| other.y == a.get_desty() && other.t != a.t))
            .filter(|&(_, a)| {
                // 3. No amphipods on the way
                !self.amphipods.iter().any(|&other| {
                    other.x == 0
                        && a.y.min(a.get_desty()) < other.y
                        && a.y.max(a.get_desty()) > other.y
                })
            })
            .map(|(i, amphi)| {
                // Count amphipods same type already in hallway
                let count = self
                    .amphipods
                    .iter()
                    .filter(|&other| other.y == amphi.get_desty())
                    .count();
                let x = (LENGTH / 4 - count) as i8;

                // Create new state
                let mut amphipods = self.amphipods.clone();

                amphipods[i].x = x;
                amphipods[i].y = amphi.get_desty();

                let distance = (amphi.y - amphi.get_desty()).abs() + x;
                let energy = self.energy + amphi.energy() * distance as u32;

                State { amphipods, energy }
            })
            .collect()
    }
}

#[aoc_generator(day23)]
fn parse(input: &str) -> Vec<Amphipod> {
    input
        .lines()
        .skip(1)
        .enumerate()
        .flat_map(|(i, l)| {
            l.chars().skip(1).enumerate().filter_map(move |(j, c)| {
                match AmphipodType::try_from(c) {
                    Some(c) => Some(Amphipod {
                        t: c,
                        x: i as i8,
                        y: j as i8,
                    }),
                    _ => None,
                }
            })
        })
        .collect::<Vec<_>>()
}

/*
 * Djikstra algorithm
 */
fn search<const LENGTH: usize>(init: State<LENGTH>) -> Option<u32> {
    let mut dist: HashMap<u128, u32> = HashMap::new();
    let mut heap: BinaryHeap<State<LENGTH>> = BinaryHeap::new();
    heap.push(init.clone());

    while let Some(node) = heap.pop() {
        if node.is_goal() {
            return Some(node.energy);
        }

        if &node.energy > dist.get(&node.get_code()).unwrap_or_else(|| &u32::MAX) {
            continue;
        }

        for successor in node.get_successors() {
            if &successor.energy < dist.get(&successor.get_code()).unwrap_or_else(|| &u32::MAX) {
                dist.insert(successor.get_code(), successor.energy);
                heap.push(successor);
            }
        }
    }

    return None;
}

#[aoc(day23, part1)]
fn part1(amphipods: &Vec<Amphipod>) -> Option<u32> {
    let board: State<8> = State::from_amphipods(amphipods);
    search(board)
}

const AMPHIPODS_FOLDED: &str = indoc::indoc! {
    "#############
    #...........#
    ###.#.#.#.###
      #D#C#B#A#
      #D#B#A#C#
      #.#.#.#.#
      #########"
};

#[aoc(day23, part2)]
fn part2(amphipods: &Vec<Amphipod>) -> Option<u32> {
    let amphipods: Vec<_> = chain!(
        amphipods.iter().cloned().filter(|a| a.x == 1),
        amphipods.iter().cloned().filter(|a| a.x == 2).map(|mut a| {
            a.x = 4;
            a
        }),
        parse(AMPHIPODS_FOLDED).into_iter(),
    )
    .collect();

    let board: State<16> = State::from_amphipods(&amphipods);
    search(board)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_sample1() {
        let sample = indoc::indoc! {
            "#############
            #.B.........#
            ###.#C#B#D###
              #A#D#C#A#
              #########"
        };

        assert_eq!(
            parse(sample),
            vec![
                Amphipod {
                    t: AmphipodType::Bronze,
                    x: 0,
                    y: 1,
                },
                Amphipod {
                    t: AmphipodType::Copper,
                    x: 1,
                    y: 4,
                },
                Amphipod {
                    t: AmphipodType::Bronze,
                    x: 1,
                    y: 6,
                },
                Amphipod {
                    t: AmphipodType::Desert,
                    x: 1,
                    y: 8,
                },
                Amphipod {
                    t: AmphipodType::Amber,
                    x: 2,
                    y: 2,
                },
                Amphipod {
                    t: AmphipodType::Desert,
                    x: 2,
                    y: 4,
                },
                Amphipod {
                    t: AmphipodType::Copper,
                    x: 2,
                    y: 6,
                },
                Amphipod {
                    t: AmphipodType::Amber,
                    x: 2,
                    y: 8,
                }
            ]
        );
    }

    #[test]
    fn test_goal() {
        let sample = indoc::indoc! {
            "#############
            #...........#
            ###A#B#C#D###
              #A#B#C#D#
              #########"
        };
        let state: State<8> = State::from_amphipods(&parse(sample));

        assert!(state.is_goal());
    }

    #[test]
    fn test_successors_1() {
        let sample = indoc::indoc! {
            "#############
            #...........#
            ###A#B#C#D###
              #A#B#C#D#
              #########"
        };
        let state: State<8> = State::from_amphipods(&parse(sample));

        assert_eq!(state.get_successors().len(), 0);
    }

    #[test]
    fn test_successors_2() {
        let sample = indoc::indoc! {
            "#############
            #.D........C#
            ###B#B#.#.###
              #A#C#A#D#
              #########"
        };
        let state: State<8> = State::from_amphipods(&parse(sample));

        assert_eq!(state.get_successors().len(), 13);
    }

    #[test]
    fn test_successors_3() {
        let sample = indoc::indoc! {
            "#############
            #.........C.#
            ###A#B#.#D###
              #A#B#C#D#
              #########"
        };
        let state: State<8> = State::from_amphipods(&parse(sample));

        assert_eq!(state.get_successors().len(), 1);
        assert_eq!(state.get_successors()[0].energy, 400);
        assert!(state.get_successors()[0].is_goal());
    }

    #[test]
    fn test_successors_4() {
        let sample = indoc::indoc! {
            "#############
            #...A.....BC#
            ###.#.#.#D###
              #A#B#C#D#
              #########"
        };
        let state: State<8> = State::from_amphipods(&parse(sample));

        assert_eq!(state.get_successors().len(), 2);
    }

    #[test]
    fn test_part1() {
        let sample = indoc::indoc! {
            "#############
            #...........#
            ###B#C#B#D###
              #A#D#C#A#
              #########"
        };

        assert_eq!(part1(&parse(sample)), Some(12521));
    }

    #[test]
    fn test_part2() {
        let sample = indoc::indoc! {
            "#############
            #...........#
            ###B#C#B#D###
              #A#D#C#A#
              #########"
        };

        assert_eq!(part2(&parse(sample)), Some(44169));
    }
}
