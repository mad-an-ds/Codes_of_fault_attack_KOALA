from gurobipy import Model, GRB, quicksum

# -----------------------------
# Parameters
# -----------------------------
N = 257

chi_offsets = [-1, 1]

# -----------------------------
# Coverage function
# -----------------------------
def coverage(k):

    # rows: k, k-3, k-10
    row_offsets = [0, -3, -10]

    # cols: 0, -3, -10
    col_offsets = [0, -3, -10]

    covered = set()

    for r in row_offsets:
        for c in col_offsets:

            # 17(k+r) + c
            b = (17 * (k + r) + c) % N

            # apply chi neighbors
            for ch in chi_offsets:
                covered.add((b + ch) % N)

    return covered


# -----------------------------
# Build subsets
# -----------------------------
subsets = [coverage(i) for i in range(N)]

universe = range(N)

# element -> covering subsets
element_to_subsets = {e: [] for e in universe}

for idx, subset in enumerate(subsets):
    for elem in subset:
        element_to_subsets[elem].append(idx)

# -----------------------------
# Create Gurobi model
# -----------------------------
m = Model("Fault_Set_Cover_N257")

# Binary variables
x = m.addVars(N, vtype=GRB.BINARY, name="x")

# -----------------------------
# Objective
# -----------------------------
m.setObjective(quicksum(x[i] for i in range(N)), GRB.MINIMIZE)

# -----------------------------
# Symmetry breaking
# -----------------------------
m.addConstr(x[0] == 1, name="symmetry")

# -----------------------------
# Coverage constraints
# -----------------------------
for elem in universe:
    m.addConstr(
        quicksum(x[i] for i in element_to_subsets[elem]) >= 1,
        name=f"cover_{elem}"
    )

# -----------------------------
# Optional solver tuning
# -----------------------------
m.Params.OutputFlag = 1
m.Params.PoolSearchMode = 2
m.Params.PoolSolutions = 10

# -----------------------------
# Solve
# -----------------------------
m.optimize()

# -----------------------------
# Extract solution
# -----------------------------
if m.Status == GRB.OPTIMAL:

    selected = [i for i in range(N) if x[i].X > 0.5]

    print("\nOptimal solution found")
    print("Minimum number of fault positions:", len(selected))
    print("Fault positions:", selected)

else:
    print("No optimal solution found")
    
    
#	Minimum number of fault positions: 21
#	Fault positions: [0, 89, 130, 143, 156, 160, 169, 173, 177, 186, 190, 197, 199, 203, 210, 216, 223, 227, 229, 240, 253]
#	Time: 27665s
