G = [[2, 4], [], [1, 2], [3], []]

n = len(G)
visited = [False]*(n+1)
pre = [0]*(n+1)
clock = 1

def dfs(u):
    global clock
    visited[u] = True
    pre[u] = clock
    clock += 1
    for v in G[u-1]:
        if not visited[v]:
            dfs(v)

dfs(1)
print(pre[1:11])
