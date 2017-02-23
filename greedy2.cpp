#include <iostream>
#include <vector>
#include "includes.hpp"
#include "common.hpp"
#include <functional>

using namespace mm;
using namespace std;

#define MAXC 1001

int V, E, R, C, X;

struct Endpoint {
    int datacenter_lat;
    vector<int> connected_caches;
    vector<int> cache_lat;
    int num_connected_caches;

    Endpoint(int dl, const vector<int>& cc, const vector<int>& cl) {
        num_connected_caches = cc.size();
        datacenter_lat = dl;
        connected_caches = cc;
        cache_lat.resize(MAXC, -1);
        for (int i = 0; i < cl.size(); ++i) {
            cache_lat[connected_caches[i]] = cl[i];
        }
    }
};

struct Request {
    int video_id, endpoint_id, num_req;
};

struct Video {
    int size;
    vector<int> request_ids;
};

vector<Video> videos;
vector<Request> requests;
vector<Endpoint> endpoints;
vector<vector<int>> videos_per_cache;

ostream& operator<<(ostream& os, Request r) {
    cout << "Request(v=" << r.video_id << ", e=" << r.endpoint_id << ", n=" << r.num_req << ")";
    return cout;
}

ostream& operator<<(ostream& os, Endpoint e) {
    cout << "Endpoint with " << e.num_connected_caches << " caches and " << e.datacenter_lat << " latency.";
    return cout;
}

vector<vector<int>> calc_savings() {
    vector<vector<int>> savings(C, vector<int>(V, 0));
    for (int c = 0; c < C; ++c) {
        for (int v = 0; v < V; ++v) {
            Video& vid = videos[v];
            if (vid.size <= X) {  // video gre v cache
                for (int rid : vid.request_ids) {  // requesti, ki zelijo ta video
                    Request& r = requests[rid];
                    if (endpoints[r.endpoint_id].cache_lat[c] >= 0)  // smo povezani z cachem c
                        savings[c][v] += r.num_req * (endpoints[r.endpoint_id].datacenter_lat -
                                                      endpoints[r.endpoint_id].cache_lat[c]);
                }
            }
        }
    }
    return savings;
}

void update_savings_for_video(vector<vector<int>>& savings, int vid) {
    for (int cid = 0; cid < C; ++cid) {
        if (savings[cid][vid] > 0) {
            savings[cid][vid] = 0;
        }
    }
    Video& v = videos[vid];
    int current_latency;
    for (int rid : v.request_ids) {
        auto& r = requests[rid];
        auto& e = endpoints[r.endpoint_id];
        current_latency = e.datacenter_lat;
        for (int cid : e.connected_caches) {
            // video is cached in cid (to be optimised)
            if (find(videos_per_cache[cid].begin(), videos_per_cache[cid].end(), vid) != videos_per_cache[cid].end()) {
                current_latency = min(current_latency, e.cache_lat[cid]);
            }
        }

        for (int cid = 0; cid < C; ++cid) {
            if (savings[cid][vid] > 0) {
                savings[cid][vid] += max(0, current_latency - e.cache_lat[cid]) * r.num_req;
            }
        }
    }
}

pair<int, int> get_best_video_to_cache(const vector<vector<int>>& savings) {
    int best_cache, best_video, best_save = -10;
    for (int c = 0; c < C; ++c) {
        for (int v = 0; v < V; ++v) {
            if (savings[c][v] >= best_save) {
                best_save = savings[c][v];
                best_cache = c;
                best_video = v;
            }
        }
    }
    return {best_cache, best_video};
}

int main() {
    cin >> V >> E >> R >> C >> X;
    videos.resize(V);
    int datacenter_latency, num_caches;
    for (int i = 0; i < V; ++i) {
        cin >> videos[i].size;
    }
    for (int e = 0; e < E; ++e) {
        cin >> datacenter_latency >> num_caches;
        vector<int> cc(num_caches), cl(num_caches);
        for (int i = 0; i < num_caches; ++i) {
            cin >> cc[i] >> cl[i];
        }
        endpoints.push_back(Endpoint(datacenter_latency, cc, cl));
    }

    requests.resize(R);
    int vid, eid, nr;
    for (int i = 0; i < R; ++i) {
        cin >> vid >> eid >> nr;
        requests[i] = {vid, eid, nr};
        videos[vid].request_ids.push_back(i);
    }

//    prn("here");
    vector<vector<int>> savings = calc_savings();
    int best_cache, best_video;
    videos_per_cache.resize(C);
    vector<int> cache_space_left(C, X);
    int done = 0;
    int all = C*V;
    while (true) {
        tie(best_cache, best_video) = get_best_video_to_cache(savings);
//        prn(best_cache);
//        prn(best_video);
//        prn(savings[best_cache][best_video]);
        if (savings[best_cache][best_video] < 0) break;
        if (cache_space_left[best_cache] < videos[best_video].size) {
            savings[best_cache][best_video] = -2;  // cant
            continue;
        }
        cache_space_left[best_cache] -= videos[best_video].size;
        videos_per_cache[best_cache].push_back(best_video);
        savings[best_cache][best_video] = -1;

        done += 1;
        if (done % 100 == 0) {
            cerr << done << "/" << all << " = " << (double) done / all << "% \n";
        }
        
        update_savings_for_video(savings, best_video);
    }

    cout << C << endl;
    for (int c = 0; c < C; ++c) {
        cout << c;
        for (int v : videos_per_cache[c]) {
            cout << " " << v;
        }
        cout << endl;
    }

    return 0;
}