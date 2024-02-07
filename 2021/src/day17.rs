use itertools::Itertools;

#[derive(Debug, PartialEq)]
pub struct Area {
    xmin: i32,
    xmax: i32,
    ymin: i32,
    ymax: i32,
}

#[aoc_generator(day17)]
pub fn parse(input: &str) -> Area {
    let (x1, x2, y1, y2) = input
        .trim()
        .trim_start_matches("target area: x=")
        .split(", y=")
        .map(|l| l.split(".."))
        .flatten()
        .map(|x| x.parse::<i32>().unwrap())
        .collect_tuple()
        .unwrap();

    Area {
        xmin: x1.min(x2),
        xmax: x1.max(x2),
        ymin: y1.min(y2),
        ymax: y1.max(y2),
    }
}

#[aoc(day17, part1)]
fn part1(area: &Area) -> i32 {
    let vmax = area.ymin.abs() - 1;
    vmax * (vmax + 1) / 2
}

#[aoc(day17, part2)]
fn part2(area: &Area) -> usize {
    (1..=area.xmax)
        .flat_map(|vx| (area.ymin..=(-area.ymin - 1)).filter(move |&vy| fire(area, vx, vy)))
        .count()
}

// @source Needed help for this one
fn fire(area: &Area, mut vx: i32, mut vy: i32) -> bool {
    let mut p = (0, 0);

    if vy >= 1 {
        for _ in 0..vy * 2 + 1 {
            p.0 += vx;
            vx -= 1;
            if p.0 > area.xmax {
                return false;
            } else if vx == 0 {
                break;
            }
        }
        vy = -vy - 1;
    }

    for (x, y, vx, _) in path(p, vx, vy) {
        if vx == 0 && x < area.xmin || x > area.xmax || y < area.ymin {
            return false;
        } else if x >= area.xmin && x <= area.xmax && y >= area.ymin && y <= area.ymax {
            return true;
        }
    }
    unreachable!();
}

fn path(p: (i32, i32), vx: i32, vy: i32) -> impl Iterator<Item = (i32, i32, i32, i32)> {
    std::iter::successors(Some((p.0, p.1, vx, vy)), |p| {
        Some((p.0 + p.2, p.1 + p.3, (p.2 - 1).max(0), p.3 - 1))
    })
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn sample1() {
        let sample = "target area: x=20..30, y=-10..-5";

        assert_eq!(
            parse(sample),
            Area {
                xmin: 20,
                xmax: 30,
                ymin: -10,
                ymax: -5
            }
        );

        assert_eq!(part1(&parse(sample)), 45);
        assert_eq!(part2(&parse(sample)), 112);
    }
}
