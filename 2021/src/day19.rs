use std::collections::HashSet;

use itertools::Itertools;
use nalgebra::{Matrix3, Rotation3, Vector3};
use rand::seq::SliceRandom;
use rand::thread_rng;

const NB_OVERLAP_BEACONS: usize = 12;
// Choose 2 pairs out of 12 beacons
const NB_OVERLAP_PAIRS: usize = NB_OVERLAP_BEACONS * (NB_OVERLAP_BEACONS - 1) / 2;

#[derive(Debug, Clone, PartialEq, Eq)]
struct Scanner {
    position: Vector3<i32>,
    rotation: Rotation3<i32>,
}

#[aoc_generator(day19)]
fn parse(input: &str) -> Vec<Vec<Vector3<i32>>> {
    input
        .split("\n\n")
        .map(|l| {
            l.lines()
                .skip(1)
                .map(|l| {
                    let mut ns = l.split(',').map(|v| v.parse::<i32>().unwrap());
                    [ns.next().unwrap(), ns.next().unwrap(), ns.next().unwrap()].into()
                })
                .collect()
        })
        .collect()
}

fn intersection(a: &Vec<(usize, usize, i32)>, b: &Vec<(usize, usize, i32)>) -> usize {
    let mut i = 0;
    let mut j = 0;
    let mut count = 0;

    while i < a.len() && j < b.len() {
        if a[i].2 == b[j].2 {
            count += 1;
            i += 1;
            j += 1;
        } else if a[i].2 < b[j].2 {
            i += 1;
        } else {
            j += 1;
        }
    }

    count
}

fn generate_rotations() -> Vec<Rotation3<i32>> {
    let r1 = Matrix3::new(0, 0, -1, 0, 1, 0, 1, 0, 0);
    let r2 = Matrix3::new(1, 0, 0, 0, 0, -1, 0, 1, 0);
    let r3 = Matrix3::new(0, -1, 0, 1, 0, 0, 0, 0, 1);

    let mut rotations: HashSet<_> = HashSet::new();

    for i in 0..4 {
        for j in 0..4 {
            for k in 0..4 {
                rotations.insert(Rotation3::from_matrix_unchecked(
                    r1.pow(i) * r2.pow(j) * r3.pow(k),
                ));
            }
        }
    }

    rotations.into_iter().collect()
}

fn get_sorted_distances(beacons: &Vec<Vector3<i32>>) -> Vec<(usize, usize, i32)> {
    beacons
        .iter()
        .enumerate()
        .tuple_combinations()
        .map(|((i, a), (j, b))| (i, j, (a - b).iter().map(|x| x.abs()).sum::<i32>()))
        .sorted_by(|(_, _, a), (_, _, b)| a.cmp(b))
        .collect::<Vec<(usize, usize, i32)>>()
}

fn get_overlapping_scanners(distances: &Vec<Vec<(usize, usize, i32)>>) -> Vec<(usize, usize)> {
    distances
        .iter()
        .enumerate()
        .tuple_combinations()
        .flat_map(|((i, a), (j, b))| {
            if intersection(&a, &b) >= NB_OVERLAP_PAIRS {
                vec![(i, j), (j, i)]
            } else {
                vec![]
            }
        })
        .collect::<Vec<(usize, usize)>>()
}

fn get_scanners_positions(
    scans: &Vec<Vec<Vector3<i32>>>,
    distances: &Vec<Vec<(usize, usize, i32)>>,
    overlapping: &Vec<(usize, usize)>,
) -> Vec<Scanner> {
    let rotations = generate_rotations();
    let mut scanners: Vec<Option<Scanner>> = vec![None; scans.len()];
    scanners[0] = Some(Scanner {
        position: Vector3::zeros(),
        rotation: Rotation3::identity(),
    });

    while scanners.iter().any(|l| l.is_none()) {
        for (i, j) in overlapping.iter() {
            // At least one not in processed
            let (i, j) = match (scanners[*i].as_ref(), scanners[*j].as_ref()) {
                (Some(_), Some(_)) => continue,
                (Some(_), None) => (*i, *j),
                (None, Some(_)) => (*j, *i),
                _ => continue,
            };

            // Find overlapping distances
            let mut candidates: Vec<_> = distances[i]
                .iter()
                .flat_map(|d| {
                    distances[j]
                        .iter()
                        .filter_map(|d2| {
                            if d.2 == d2.2 {
                                Some((scans[i][d.0], scans[i][d.1], scans[j][d2.0], scans[j][d2.1]))
                            } else {
                                None
                            }
                        })
                        .collect::<Vec<_>>()
                })
                .collect();

            // Shuffle
            // @note : this is not optimal, but it works
            candidates.shuffle(&mut thread_rng());
            let (xi, yi, xj, yj) = candidates.pop().unwrap();

            let si = scanners[i].as_ref().unwrap();
            let xi = si.rotation * xi + si.position;
            let yi = si.rotation * yi + si.position;

            // Find the rotation that matches the overlapping distances
            if let Some((p, r)) = rotations.iter().find_map(|r| {
                let xj = r * xj;
                let yj = r * yj;

                if (xi - xj) == (yi - yj) {
                    Some((xi - xj, r.clone()))
                } else if (xi - yj) == (yi - xj) {
                    Some((xi - yj, r.clone()))
                } else {
                    None
                }
            }) {
                scanners[j] = Some(Scanner {
                    position: p,
                    rotation: r,
                });
            }
        }
    }

    scanners.into_iter().map(|x| x.unwrap()).collect()
}

#[aoc(day19, part1)]
fn part1(scans: &Vec<Vec<Vector3<i32>>>) -> usize {
    // Compute all distances of any two points for each list of beacons
    // Do not use same point twice
    let distances = scans
        .iter()
        .map(|beacons| get_sorted_distances(beacons))
        .collect::<Vec<_>>();

    // Find overlapping beacons
    let overlapping = get_overlapping_scanners(&distances);

    // Get scanners positions
    let scanners = get_scanners_positions(scans, &distances, &overlapping);

    // Number beacons
    scans
        .iter()
        .enumerate()
        .fold(HashSet::new(), |mut acc, (i, beacons)| {
            beacons.iter().for_each(|p| {
                acc.insert(scanners[i].rotation * p + scanners[i].position);
            });
            acc
        })
        .len()
}

#[aoc(day19, part2)]
fn part2(scans: &Vec<Vec<Vector3<i32>>>) -> i32 {
    let distances = scans
        .iter()
        .map(|beacons| get_sorted_distances(beacons))
        .collect::<Vec<_>>();

    let overlapping = get_overlapping_scanners(&distances);

    let scanners = get_scanners_positions(scans, &distances, &overlapping);

    scanners
        .iter()
        .tuple_combinations()
        .map(|(a, b)| {
            (a.position - b.position)
                .iter()
                .map(|x| x.abs())
                .sum::<i32>()
        })
        .max()
        .unwrap()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_sample() {
        let sample = indoc::indoc! {
            "--- scanner 0 ---
            404,-588,-901
            528,-643,409
            -838,591,734
            390,-675,-793
            -537,-823,-458
            -485,-357,347
            -345,-311,381
            -661,-816,-575
            -876,649,763
            -618,-824,-621
            553,345,-567
            474,580,667
            -447,-329,318
            -584,868,-557
            544,-627,-890
            564,392,-477
            455,729,728
            -892,524,684
            -689,845,-530
            423,-701,434
            7,-33,-71
            630,319,-379
            443,580,662
            -789,900,-551
            459,-707,401

            --- scanner 1 ---
            686,422,578
            605,423,415
            515,917,-361
            -336,658,858
            95,138,22
            -476,619,847
            -340,-569,-846
            567,-361,727
            -460,603,-452
            669,-402,600
            729,430,532
            -500,-761,534
            -322,571,750
            -466,-666,-811
            -429,-592,574
            -355,545,-477
            703,-491,-529
            -328,-685,520
            413,935,-424
            -391,539,-444
            586,-435,557
            -364,-763,-893
            807,-499,-711
            755,-354,-619
            553,889,-390

            --- scanner 2 ---
            649,640,665
            682,-795,504
            -784,533,-524
            -644,584,-595
            -588,-843,648
            -30,6,44
            -674,560,763
            500,723,-460
            609,671,-379
            -555,-800,653
            -675,-892,-343
            697,-426,-610
            578,704,681
            493,664,-388
            -671,-858,530
            -667,343,800
            571,-461,-707
            -138,-166,112
            -889,563,-600
            646,-828,498
            640,759,510
            -630,509,768
            -681,-892,-333
            673,-379,-804
            -742,-814,-386
            577,-820,562

            --- scanner 3 ---
            -589,542,597
            605,-692,669
            -500,565,-823
            -660,373,557
            -458,-679,-417
            -488,449,543
            -626,468,-788
            338,-750,-386
            528,-832,-391
            562,-778,733
            -938,-730,414
            543,643,-506
            -524,371,-870
            407,773,750
            -104,29,83
            378,-903,-323
            -778,-728,485
            426,699,580
            -438,-605,-362
            -469,-447,-387
            509,732,623
            647,635,-688
            -868,-804,481
            614,-800,639
            595,780,-596

            --- scanner 4 ---
            727,592,562
            -293,-554,779
            441,611,-461
            -714,465,-776
            -743,427,-804
            -660,-479,-426
            832,-632,460
            927,-485,-438
            408,393,-506
            466,436,-512
            110,16,151
            -258,-428,682
            -393,719,612
            -211,-452,876
            808,-476,-593
            -575,615,604
            -485,667,467
            -680,325,-822
            -627,-443,-432
            872,-547,-609
            833,512,582
            807,604,487
            839,-516,451
            891,-625,532
            -652,-548,-490
            30,-46,-14"
        };

        assert_eq!(part1(&parse(sample)), 79);
        assert_eq!(part2(&parse(sample)), 3621);
    }
}
