import matplotlib.pyplot as plt

# Load data
track_x, track_y = [], []
car_x, car_y = [], []

# Load track points
with open("track.csv") as f:
    for line in f:
        x, y = map(float, line.strip().split(","))
        track_x.append(x)
        track_y.append(y)

# Load car path
with open("car_path.csv") as f:
    for line in f:
        x, y = map(float, line.strip().split(","))
        car_x.append(x)
        car_y.append(y)

# Plot
plt.figure(figsize=(8, 6))
plt.plot(track_x, track_y, 'k--', label="Track")         
plt.plot(car_x, car_y, 'b-', label="Vehicle Path")      
plt.plot(car_x[0], car_y[0], 'go', label="Start")      
plt.plot(car_x[-2], car_y[-2], 'ro', label="End")     
plt.legend()
plt.axis("equal")
plt.show()
