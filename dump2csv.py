
#!/usr/bin/env python3
"""Convert a raw `logdata` partition dump into a CSV file.

Usage:
    python3 dump_to_csv.py logdata.bin            # writes logdata.csv
    python3 dump_to_csv.py logdata.bin out.csv

Record layout (16 bytes, little-endian), struct format "<H5BhHHB2x":
    year (u16), month, day, hour, minute, second (u8 each),
    temp (i16, 0.01 C), hum (u16, 0.01 %), press_or_seq (u16), flag (u8), 2 bytes padding
"""
import csv
import struct
import sys

FMT = "<H5BhHHB2x"
REC = struct.calcsize(FMT)          # must be 16
ERASED = b"\xff" * REC


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)

    src = sys.argv[1]
    dst = sys.argv[2] if len(sys.argv) > 2 else src.rsplit(".", 1)[0] + ".csv"

    data = open(src, "rb").read()
    if len(data) % REC:
        print(f"warning: file size {len(data)} is not a multiple of {REC}, the tail is ignored")

    rows, erased, prev_seq, breaks = [], 0, None, []

    for off in range(0, len(data) - REC + 1, REC):
        raw = data[off:off + REC]
        if raw == ERASED:                       # empty (never written) space
            erased += 1
            continue

        year, mo, day, h, mi, s, temp, hum, val, flag = struct.unpack(FMT, raw)
        rows.append({
            "index": off // REC,
            "time": f"{year:04d}-{mo:02d}-{day:02d} {h:02d}:{mi:02d}:{s:02d}",
            "temp_c": temp / 100,
            "hum_pct": hum / 100,
            "press_or_seq": val,
            "flag": flag,
        })

        # Sequence check only makes sense while the field holds a counter.
        if prev_seq is not None and val != prev_seq + 1:
            breaks.append((prev_seq, val, off // REC))
        prev_seq = val

    with open(dst, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=["index", "time", "temp_c", "hum_pct", "press_or_seq", "flag"])
        w.writeheader()
        w.writerows(rows)

    print(f"records decoded : {len(rows)}")
    print(f"erased records  : {erased}")
    print(f"written to      : {dst}")
    if breaks:
        print(f"value breaks (prev -> next @index): {breaks[:10]}{' ...' if len(breaks) > 10 else ''}")
    else:
        print("value breaks    : none (press_or_seq grows by 1 without gaps)")


if __name__ == "__main__":
    main()