from sys import argv, exit

if len(argv) < 3:
    print("Not enough arguments")
    print("Gimme: scoring.py input.in result.out")
    exit(0)

inf = open(argv[1])
outf = open(argv[2])

v, e, r, c, x = map(int, inf.readline().split())

# [video_id] -> size in MB
v_len = list(map(int, inf.readline().split()))

# Datacenter latency [node_id] -> latency in ms
lat_dat = []
# Cache latency [node_id][cache_id] -> latency in ms
lat_ca = []

# READ latencies
for i in range(e):
    ld, k = map(int, inf.readline().split())
    lat_dat.append(ld)
    lat_ca.append([ld for _ in range(c)])
    for j in range(k):
        cid, lc = map(int, inf.readline().split())
        lat_ca[-1][cid] = lc

# [request_id] -> (video_id, node_id, num_requests)
req = []
for i in range(r):
    req.append(list(map(int, inf.readline().split())))


# ========== READING RESULTS ===============
cached = [None for _ in range(c)]
num_cache = int(outf.readline())
if num_cache > c or num_cache < 0:
    print("You are trying to use more caches than are available! No can do")
    exit(1)
for i in range(num_cache):
    line = list(map(int, outf.readline().split()))
    cid = line[0]
    if cached[cid] is not None:
        print("You are trying to use the cache server #{} multiple times".format(cid))
        exit(1)
    cached[cid] = set(line[1:])
    csize = sum(v_len[vid] for vid in line[1:])
    if csize > x:
        print("You are trying to store {}MB (X={}MB) in cahce #{}".format(csize, x, cid))
        exit(1)
# Swap None for empty set
cached = [cac or set() for cac in cached]


# ========= Start scoring ==========
score = 0

for vid, nid, num in req:
    worst = lat_dat[nid]
    best = worst
    for i, cac in enumerate(cached):
        if vid in cac:
            best = min(best, lat_ca[nid][i])
    score += (worst - best) * num

score *= 1000 / sum(num for _, _, num in req)
print("Your score is", int(score))
