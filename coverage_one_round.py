import gurobipy as gp
from gurobipy import GRB

n = 257


#### for round 1 faults
# def covered_outputs(k):
#     return [
#         (k + 1) % n,
#         (k - 1) % n,
#         (k - 2) % n,
#         (k - 4) % n,
#         (k - 9) % n,
#         (k - 11) % n
#     ]


#### for round 2 faults
def covered_outputs(k):
    return [
        (17 * k - 1) % n,
        (17 * k + 1) % n,
        (17 * k - 3 - 1) % n,
        (17 * k - 3 + 1) % n,
        (17 * k - 10 - 1) % n,
        (17 * k - 10 + 1) % n,
        (17 * (k - 3) - 1) % n,
        (17 * (k - 3) + 1) % n,
        (17 * (k - 3) - 3 - 1) % n,
        (17 * (k - 3) - 3 + 1) % n,
        (17 * (k - 3) - 10 - 1) % n,
        (17 * (k - 3) - 10 + 1) % n,
        (17 * (k - 10) - 1) % n,
        (17 * (k - 10) + 1) % n,
        (17 * (k - 10) - 3 - 1) % n,
        (17 * (k - 10) - 3 + 1) % n,
        (17 * (k - 10) - 10 - 1) % n,
        (17 * (k - 10) - 10 + 1) % n
    ]

coverage = {k: covered_outputs(k) for k in range(n)}

covered_by = {j: [] for j in range(n)}
for k in range(n):
    for j in coverage[k]:
        covered_by[j].append(k)

m = gp.Model("min_output_cover")

x = m.addVars(n, vtype=GRB.BINARY, name="x")

for j in range(n):
    m.addConstr(gp.quicksum(x[k] for k in covered_by[j]) >= 1)

m.setObjective(gp.quicksum(x[k] for k in range(n)), GRB.MINIMIZE)

# ---- Callback ----
def mycallback(model, where):
    if where == GRB.Callback.MIPSOL:
        obj = model.cbGet(GRB.Callback.MIPSOL_OBJ)

        # Check if objective reached 52
        if int(obj) == 52:
            print("\n⚡ Found intermediate solution with objective = 52")

            sol = model.cbGetSolution(x)
            selected = [k for k in range(n) if sol[k] > 0.5]

            print("Selected bits:", selected)

            # Optional: stop after finding this solution
            # model.terminate()

# Optimize with callback
m.optimize(mycallback)
