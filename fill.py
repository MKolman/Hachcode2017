"""
    This file tries to fill as many youtube videos as pissible in any of the
    caches, starting with those that have more requests from any node.
    Usage:
        python3 fill.py < example.in > example.out
"""
# =========== READ INPUT =============
v, e, r, c, x = map(int, input().split())

# [video_id] -> size in MB
v_len = list(map(int, input().split()))

# Datacenter latency [node_id] -> latency in ms
lat_dat = []
# Cache latency [node_id][cache_id] -> latency in ms
lat_ca = []

# READ latencies
for i in range(e):
    ld, k = map(int, input().split())
    lat_dat.append(ld)
    lat_ca.append([ld for _ in range(c)])
    for j in range(k):
        cid, lc = map(int, input().split())
        lat_ca[-1][cid] = lc

video_price = [0 for _ in range(v)]

# [request_id] -> (video_id, node_id, num_requests)
req = []
for i in range(r):
    req.append(list(map(int, input().split())))
    vid, nid, num = req[-1]
    video_price[vid] += num


# CALCULATE

lens = list(zip(video_price, v_len, range(v)))
lens.sort()

left = 0
cached = [[] for _ in range(c)]
space_left = [x for _ in range(c)]
cid = 0
for _, size, i in lens[::-1]:
    for j in range(c):
        j %= c
        if space_left[j] >= size:
            space_left[j] -= size
            cached[j].append(i)
            break
    else:
        left += 1
    cid += 1
# print(left)
print(c)
for i, vid in enumerate(cached):
    print(i, " ".join(map(str, vid)))
