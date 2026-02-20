import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('benchmark/benchmark_results.csv')
plt.plot(df['n'], df['time_ms'], marker='o')
plt.xlabel('Number of Jobs (n)')
plt.ylabel('Time (ms)')
# plt.title('Benchmark Tower_Schedule')
plt.grid(True)
plt.savefig('benchmark/plot_benchmark.png')
plt.show()
