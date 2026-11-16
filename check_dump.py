import struct, sys

d = open(sys.argv[1], "rb").read()
REC = 16
fmt = "<H5BhHHB2x"                  # рік, 5 байт часу, темп, волога, тиск/seq, прапор
erased = b"\xff" * REC

seqs, stop = [], None
for i in range(0, len(d), REC):
    raw = d[i:i+REC]
    if raw == erased:
        stop = i // REC
        break
    year, mo, day, h, mi, s, temp, hum, seq, flag = struct.unpack(fmt, raw)
    seqs.append(seq)

print("records before first 0xFF:", len(seqs), "of", len(d) // REC)
if stop is not None:
    tail = d[stop*REC:]
    print("everything after is 0xFF:", tail == b"\xff" * len(tail))

gaps = [(seqs[i-1], seqs[i]) for i in range(1, len(seqs)) if seqs[i] != seqs[i-1] + 1]
print("first seq:", seqs[0], " last seq:", seqs[-1])
print("breaks in sequence:", gaps if gaps else "none")