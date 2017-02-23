#include <iostream>
#include <vector>
#include "includes.hpp"
#include "common.hpp"

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

ostream& operator<<(ostream& os, Request r) {
    cout << "Request(v=" << r.video_id << ", e=" << r.endpoint_id << ", n=" << r.num_req << ")";
    return cout;
}

ostream& operator<<(ostream& os, Endpoint e) {
    cout << "Endpoint with " << e.num_connected_caches << " caches and " << e.datacenter_lat << " latency.";
    return cout;
}

int main() {
    cin >> V >> E >> R >> C >> X;
    videos.resize(V);
    int datacenter_latency, num_caches;
    for (int i = 0; i < V; ++i) {
        cin >> videos[i].size;
    }
    vector<Endpoint> endpoints;
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

    return 0;
}

