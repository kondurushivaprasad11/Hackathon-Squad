#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

// Check whether two coders conflict
bool hasConflict(int u, int v,
                 const vector<vector<int>> &graph)
{
    // Search smaller adjacency list for faster lookup
    if(graph[u].size() > graph[v].size())
    {
        return binary_search(
            graph[v].begin(),
            graph[v].end(),
            u
        );
    }

    return binary_search(
        graph[u].begin(),
        graph[u].end(),
        v
    );
}

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int N, M;

    if(!(cin >> N >> M))
        return 0;

    // skill[i] stores skill rating of ith coder
    vector<long long> skill(N + 1);

    for(int i = 1; i <= N; i++)
    {
        cin >> skill[i];
    }

    // Undirected conflict graph
    vector<vector<int>> graph(N + 1);

    for(int i = 0; i < M; i++)
    {
        int u, v;
        cin >> u >> v;

        graph[u].push_back(v);
        graph[v].push_back(u);
    }

    // Required for binary search conflict checking
    for(int i = 1; i <= N; i++)
    {
        sort(graph[i].begin(), graph[i].end());
    }

    /*
        Phase 1 : Greedy Initial Team

        Priority formula:

            skill / sqrt(degree + 1)

        High skill coders are rewarded.
        Highly conflicting coders are penalized.
    */

    vector<pair<double,int>> ranking;

    ranking.reserve(N);

    for(int i = 1; i <= N; i++)
    {
        double priority =
            (double)skill[i] /
            sqrt((double)graph[i].size() + 1.0);

        ranking.push_back({priority, i});
    }

    // Highest priority first
    sort(ranking.rbegin(), ranking.rend());

    vector<bool> selected(N + 1, false);
    vector<bool> blocked(N + 1, false);

    vector<int> team;

    long long totalSkill = 0;

    for(auto &[priority, coder] : ranking)
    {
        // Safe to include
        if(!blocked[coder])
        {
            selected[coder] = true;

            team.push_back(coder);

            totalSkill += skill[coder];

            // Block all rivals
            for(int enemy : graph[coder])
            {
                blocked[enemy] = true;
            }
        }
    }

    /*
        Helper function to build candidate pool

        Returns highest skill non-selected coders.
    */

    auto buildPool = [&](int limit)
    {
        vector<pair<long long,int>> temp;

        for(int i = 1; i <= N; i++)
        {
            if(!selected[i])
            {
                temp.push_back({skill[i], i});
            }
        }

        sort(temp.rbegin(), temp.rend());

        vector<int> pool;

        for(int i = 0;
            i < (int)temp.size() && i < limit;
            i++)
        {
            pool.push_back(temp[i].second);
        }

        return pool;
    };

    /*
        Phase 2 : Single Replacement Optimization

        Try replacing one selected coder
        with one stronger compatible coder.
    */

    const int SWAP_POOL = 600;

    for(int pass = 0; pass < 2; pass++)
    {
        bool improved = false;

        // Rebuild pool after every pass
        vector<int> phase2Pool =
            buildPool(SWAP_POOL);

        for(int &member : team)
        {
            if(member == -1)
                continue;

            long long removedSkill =
                skill[member];

            for(int cand : phase2Pool)
            {
                if(selected[cand])
                    continue;

                if(skill[cand] <= removedSkill)
                    continue;

                bool conflict = false;

                // Candidate should fit with remaining team
                for(int enemy : graph[cand])
                {
                    if(selected[enemy] &&
                       enemy != member)
                    {
                        conflict = true;
                        break;
                    }
                }

                if(conflict)
                    continue;

                // Apply swap
                selected[member] = false;

                selected[cand] = true;

                totalSkill +=
                    skill[cand] - removedSkill;

                member = cand;

                improved = true;

                break;
            }
        }

        // Stop early if no improvement occurs
        if(!improved)
            break;
    }

    /*
        Phase 3 : 1-for-2 Augmentation

        Replace one weaker coder
        with two compatible stronger coders.
    */

    const int POOL_SIZE = 400;

    vector<int> pool =
        buildPool(POOL_SIZE);

    vector<int> added;

    for(int &member : team)
    {
        if(member == -1)
            continue;

        long long removedSkill =
            skill[member];

        bool swapped = false;

        for(size_t i = 0;
            i < pool.size() && !swapped;
            i++)
        {
            int a = pool[i];

            if(selected[a])
                continue;

            bool conflictA = false;

            for(int enemy : graph[a])
            {
                if(selected[enemy] &&
                   enemy != member)
                {
                    conflictA = true;
                    break;
                }
            }

            if(conflictA)
                continue;

            for(size_t j = i + 1;
                j < pool.size() && !swapped;
                j++)
            {
                int b = pool[j];

                if(selected[b])
                    continue;

                // A and B should not conflict
                if(hasConflict(a, b, graph))
                    continue;

                bool conflictB = false;

                for(int enemy : graph[b])
                {
                    if(selected[enemy] &&
                       enemy != member)
                    {
                        conflictB = true;
                        break;
                    }
                }

                if(conflictB)
                    continue;

                long long gain =
                    skill[a] +
                    skill[b] -
                    removedSkill;

                if(gain <= 0)
                    continue;

                // Apply replacement
                selected[member] = false;

                selected[a] = true;
                selected[b] = true;

                totalSkill += gain;

                member = -1;

                added.push_back(a);
                added.push_back(b);

                swapped = true;
            }
        }
    }

    // Add newly selected coders
    for(int x : added)
    {
        team.push_back(x);
    }

    /*
        Phase 4 : Free Insertions

        Some coders may become insertable
        after earlier swaps remove conflicts.
    */

    for(int i = 1; i <= N; i++)
    {
        if(selected[i])
            continue;

        bool conflict = false;

        for(int enemy : graph[i])
        {
            if(selected[enemy])
            {
                conflict = true;
                break;
            }
        }

        // Pure gain
        if(!conflict)
        {
            selected[i] = true;

            totalSkill += skill[i];

            team.push_back(i);
        }
    }

    // Build final valid team
    vector<int> finalTeam;

    for(int coder : team)
    {
        if(coder != -1)
        {
            finalTeam.push_back(coder);
        }
    }

    sort(finalTeam.begin(), finalTeam.end());

    // Final output
    cout << totalSkill << "\n";

    for(size_t i = 0; i < finalTeam.size(); i++)
    {
        if(i)
            cout << " ";

        cout << finalTeam[i];
    }

    cout << "\n";

    return 0;
}
