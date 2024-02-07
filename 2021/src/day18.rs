//@note : Should have used LinkedList instead of Vec
// But I'm not sure if it would have been faster

use itertools::Itertools;

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct SnailNum(Vec<(u32, u32)>);

impl SnailNum {
    fn from_str(input: &str) -> SnailNum {
        let (v, d) = input
            .as_bytes()
            .iter()
            .fold((Vec::new(), 0), |(mut v, mut depth), b| {
                match b {
                    b'[' => depth += 1,
                    b']' => depth -= 1,
                    b'0'..=b'9' => v.push(((*b - b'0') as u32, depth as u32)),
                    b',' => (),
                    _ => panic!("Invalid character"),
                }
                (v, depth)
            });

        if d != 0 {
            panic!("Invalid depth");
        }

        SnailNum(v)
    }

    fn explode(&mut self) -> bool {
        if let Some((index, _)) = self.0.iter().enumerate().find(|(_, &(_, d))| d == 5) {
            let (v1, d) = self.0[index];
            let (v2, _) = self.0[index + 1];

            self.0.remove(index);
            self.0[index] = (0, d - 1);
            self.0
                .get_mut(index.wrapping_sub(1))
                .map(|v: &mut _| v.0 += v1);
            self.0.get_mut(index + 1).map(|v: &mut _| v.0 += v2);

            true
        } else {
            false
        }
    }

    fn split(&mut self) -> bool {
        if let Some((index, _)) = self.0.iter().enumerate().find(|(_, &(v, _))| v >= 10) {
            let (v, d) = self.0[index];
            self.0[index] = (v / 2, d + 1);
            self.0.insert(index + 1, (v / 2 + v % 2, d + 1));

            true
        } else {
            false
        }
    }

    fn reduce(&mut self) {
        while self.explode() || self.split() {}
    }

    fn add(&mut self, other: &SnailNum) {
        self.0.extend(other.0.iter().cloned());
        self.0.iter_mut().for_each(|(_, d)| *d += 1);
        self.reduce();
    }

    fn magnitude(&self) -> u32 {
        let mut v = self.0.clone();
        let max_depth = *v.iter().map(|(_, d)| d).max().unwrap();

        for depth in (1..=max_depth).rev() {
            if v.len() <= 1 {
                break;
            }

            let mut i = 0;
            while i < v.len() {
                if v[i].1 == depth {
                    v[i] = (v[i].0 * 3 + v[i + 1].0 * 2, depth - 1);
                    v.remove(i + 1);
                }
                i += 1;
            }
        }

        v[0].0
    }
}

#[aoc_generator(day18)]
fn parse(input: &str) -> Vec<SnailNum> {
    input.lines().map(|l| SnailNum::from_str(l)).collect()
}

#[aoc(day18, part1)]
fn part1(snailnums: &Vec<SnailNum>) -> usize {
    let init = snailnums.first().unwrap();

    let f = snailnums.iter().skip(1).fold(init.clone(), |mut acc, sn| {
        acc.add(sn);
        acc
    });

    f.magnitude() as usize
}

#[aoc(day18, part2)]
fn part2(snailnums: &Vec<SnailNum>) -> usize {
    (0..snailnums.len())
        .cartesian_product(0..snailnums.len())
        .filter(|&(i, j)| i != j)
        .map(|(i, j)| {
            let mut acc = snailnums[i].clone();
            acc.add(&snailnums[j]);
            acc.magnitude() as usize
        })
        .fold(0, |acc, x| acc.max(x))
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parsing() {
        assert_eq!(
            SnailNum::from_str("[[1,2],[[3,4],5]]"),
            SnailNum(vec![(1, 2), (2, 2), (3, 3), (4, 3), (5, 2)])
        );
    }

    #[test]
    fn test_magnitude() {
        assert_eq!(SnailNum::from_str("[[1,2],[[3,4],5]]").magnitude(), 143);
        assert_eq!(
            SnailNum::from_str("[[[[0,7],4],[[7,8],[6,0]]],[8,1]]").magnitude(),
            1384
        );
        assert_eq!(
            SnailNum::from_str("[[[[5,0],[7,4]],[5,5]],[6,6]]").magnitude(),
            1137
        );
    }

    #[test]
    fn test_reduce_1() {
        let mut a = SnailNum::from_str("[[[[[9,8],1],2],3],4]");
        let b = SnailNum::from_str("[[[[0,9],2],3],4]");

        a.reduce();
        assert_eq!(a, b)
    }

    #[test]
    fn test_reduce_2() {
        let mut a = SnailNum::from_str("[[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]]");
        let b = SnailNum::from_str("[[3,[2,[8,0]]],[9,[5,[7,0]]]]");

        a.reduce();
        assert_eq!(a, b)
    }

    #[test]
    fn test_add() {
        let mut a = SnailNum::from_str("[[[[4,3],4],4],[7,[[8,4],9]]]");
        a.add(&SnailNum::from_str("[1,1]"));
        assert_eq!(a, SnailNum::from_str("[[[[0,7],4],[[7,8],[6,0]]],[8,1]]"));
    }

    #[test]
    fn test_sample() {
        let input = indoc::indoc! {
            "[[[0,[5,8]],[[1,7],[9,6]]],[[4,[1,2]],[[1,4],2]]]
            [[[5,[2,8]],4],[5,[[9,9],0]]]
            [6,[[[6,2],[5,6]],[[7,6],[4,7]]]]
            [[[6,[0,7]],[0,9]],[4,[9,[9,0]]]]
            [[[7,[6,4]],[3,[1,3]]],[[[5,5],1],9]]
            [[6,[[7,3],[3,2]]],[[[3,8],[5,7]],4]]
            [[[[5,4],[7,7]],8],[[8,3],8]]
            [[9,3],[[9,9],[6,[4,9]]]]
            [[2,[[7,7],7]],[[5,8],[[9,3],[0,2]]]]
            [[[[5,2],5],[8,[3,7]]],[[5,[7,5]],[4,4]]]"
        };

        assert_eq!(part1(&parse(input)), 4140);
        assert_eq!(part2(&parse(input)), 3993);
    }
}
