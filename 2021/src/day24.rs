use lazy_static::lazy_static;
use regex::Regex;

/*
  Common pattern in the execution :
  1. x = ((z mod 26) + xincr) != z
  2. z = floor(z / optype)
  3. z = z * (25 * x + 1)
  4. z = z + ((w + offset) * x)
*/
lazy_static! {
    static ref REG: Regex = Regex::new(
        r"
mul x 0
add x z
mod x 26
div z (26|1)
add x (-?[0-9]+)
eql x w
eql x 0
mul y 0
add y 25
mul y x
add y 1
mul z y
mul y 0
add y w
add y (-?[0-9]+)
mul y x
add z y",
    )
    .unwrap();
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
struct Instr {
    optype: i32,
    xincr: i32,
    yincr: i32,
}

fn parse_instr(input: &str) -> Option<Instr> {
    REG.captures(input).and_then(|r| {
        Some(Instr {
            optype: r[1].parse().unwrap(),
            xincr: r[2].parse().unwrap(),
            yincr: r[3].parse().unwrap(),
        })
    })
}

#[aoc_generator(day24)]
fn parse(input: &str) -> Vec<Instr> {
    input
        .split("inp w")
        .skip(1)
        .map(parse_instr)
        .map(|i| {
            i.unwrap_or_else(|| {
                panic!("Input not valid");
            })
        })
        .collect()
}

// @note Needed help from Reddit on that one...
fn stack(instructions: &[Instr], part_one: bool) -> String {
    instructions
        .iter()
        .enumerate()
        .fold(
            (vec![], vec![0; 14]),
            |(mut stack, mut result), (i, inst)| {
                if inst.optype == 1 {
                    stack.push((i, inst.yincr));
                } else {
                    let (ia, a) = stack.pop().unwrap();
                    let diff = a + inst.xincr;
                    result[ia] = if part_one {
                        9.min(9 - diff)
                    } else {
                        1.max(1 - diff)
                    };
                    result[i] = if part_one {
                        9.min(9 + diff)
                    } else {
                        1.max(1 + diff)
                    };
                }
                (stack, result)
            },
        )
        .1
        .iter()
        .map(|&x| char::from_digit(x as u32, 10).unwrap())
        .collect()
}

#[aoc(day24, part1)]
fn part1(instructions: &[Instr]) -> String {
    stack(instructions, true)
}

#[aoc(day24, part2)]
fn part2(instructions: &[Instr]) -> String {
    stack(instructions, false)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse() {
        let sample = indoc::indoc! {
            "inp w
            mul x 0
            add x z
            mod x 26
            div z 1
            add x 12
            eql x w
            eql x 0
            mul y 0
            add y 25
            mul y x
            add y 1
            mul z y
            mul y 0
            add y w
            add y 1
            mul y x
            add z y"
        };

        assert_eq!(
            parse_instr(sample),
            Some(Instr {
                optype: 1,
                xincr: 12,
                yincr: 1
            })
        );
    }
}
