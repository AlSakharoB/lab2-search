import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('search_times.csv')

plt.figure()
for col in ['linear_us','bst_us','rbt_us','hash_us','multimap_us']:
    plt.plot(df['size'], df[col], label=col.split('_')[0])
plt.xscale('log'); plt.yscale('log')
plt.xlabel('Размер массива'); plt.ylabel('Время поиска, мкс')
plt.title('Сравнение алгоритмов поиска'); plt.grid(True, which='both')
plt.legend(); plt.savefig('search_time_compare.png', dpi=200)

plt.figure()
plt.plot(df['size'], df['collisions'])
plt.xscale('log'); plt.xlabel('Размер массива')
plt.ylabel('Колизии'); plt.title('Коллизии хеш-функции')
plt.grid(True, which='both'); plt.savefig('hash_collisions.png', dpi=200)