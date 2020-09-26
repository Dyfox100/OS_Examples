import re
import sys

old_string = sys.argv[2]
new_string = sys.argv[3]
pid = int(sys.argv[1])

maps_file_path = "/proc/{}/maps".format(pid)
mem_file_path = "/proc/{}/mem".format(pid)

maps_file = open(maps_file_path, "r")
mem_file = open(mem_file_path, "r+b")

for mem_region_line in maps_file:
    if "heap" in mem_region_line:
        heap_addrs = mem_region_line.split()[0]
        heap_addrs = heap_addrs.split('-')
        heap_start = int(heap_addrs[0], 16)
        heap_end = int(heap_addrs[1], 16)
        break

mem_file.seek(heap_start)
all_heap = mem_file.read(heap_end - heap_start)

for match in re.finditer(bytes(old_string, "ASCII"), all_heap):
    mem_file.seek(heap_start + match.start())
    mem_file.write(bytes(new_string, "ASCII"))

mem_file.close()
maps_file.close()
