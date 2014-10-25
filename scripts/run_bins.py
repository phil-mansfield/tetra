import os

bin_name = "bin/bin_dimensions"
loc_fmt = "/home/diemer/Simulations/Box_L%s_N1024_CBol/Snaps/snapdir_%s/snapshot_%s.0"

widths = ["0063", "0250", "2000"]
times = ["0", "50", "100"]

for time in times:
    for width in widths:
        loc = loc_fmt % (width, time, time)
        os.system("%s %s > output/dim_bin_%s_%s.out" % (bin_name, loc, time, width))
