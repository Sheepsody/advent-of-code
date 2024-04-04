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

#[derive(Clone, Eq, PartialEq)]
struct State {
    energy: u32,
    amphipods: [Amphipod; 8],
}

impl Ord for State {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        other.energy.cmp(&self.energy)
    }
}

impl PartialOrd for State {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(other.energy.cmp(&self.energy))
    }
}

impl From<&str> for State {
    fn from(input: &str) -> Self {
        State {
            energy: 0,
            amphipods: input
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
                .try_into()
                .unwrap(),
        }
    }
}

impl State {
    fn is_goal(&self) -> bool {
        self.amphipods
            .iter()
            .all(|amphi| (amphi.x != 0) && (amphi.y == amphi.get_desty()))
    }

    fn get_code(&self) -> u64 {
        self.amphipods.iter().fold(0, |acc, amphi| {
            (acc << 8) + ((amphi.x as u64) << 4) + (amphi.y as u64)
        })
    }

    fn get_successors(&self) -> Vec<Self> {
        self.to_hallway()
            .into_iter()
            .chain(self.to_room())
            .collect()
    }

    // TODO : Fix doc
    // Rules
    // 1. Never stop space outside room
    // 2. Stay same spot hallway until can enter room
    fn to_hallway(&self) -> Vec<Self> {
        self.amphipods
            .iter()
            .enumerate()
            .filter(|(_, amphi)| {
                let mut can_move = false;
                // x == 1 && not dest
                can_move |= amphi.x == 1 && amphi.y != amphi.get_desty();
                // x == 1 && other amphipods in the room not in dest
                can_move |= amphi.x == 1
                    && self
                        .amphipods
                        .iter()
                        .any(|a| a.y == amphi.y && a.x == 2 && a.y != a.get_desty());
                // x == 2 && not dest && no other amphipods in the room
                can_move |= amphi.x == 2
                    && amphi.y != amphi.get_desty()
                    && self.amphipods.iter().all(|a| a.y != amphi.y || a.x != 1);

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

    // Rules
    // 1. never move from the hallway into a room unless that room is their destination
    // 2. That room contains no amphipods which do not also have that room as their destination
    // _Note:_ Works for any size of hallway
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
                let x = 2 - count as i8;

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
fn parse(input: &str) -> State {
    State::from(input)
}

/*
 * Djikstra algorithm
 */
#[aoc(day23, part1)]
fn part1(board: &State) -> Option<u32> {
    let mut dist: HashMap<u64, u32> = HashMap::new();
    let mut heap: BinaryHeap<State> = BinaryHeap::new();
    heap.push(board.clone());

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
            State::from(sample),
            State {
                cost: 0,
                amphipods: [
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
            }
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

        assert!(parse(sample).is_goal());
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

        assert_eq!(parse(sample).get_successors().len(), 0);
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

        assert_eq!(parse(sample).get_successors().len(), 13);
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

        assert_eq!(parse(sample).get_successors().len(), 1);
        assert_eq!(parse(sample).get_successors()[0].cost, 400);
        assert!(parse(sample).get_successors()[0].is_goal());
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

        assert_eq!(parse(sample).get_successors().len(), 2);
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
}
