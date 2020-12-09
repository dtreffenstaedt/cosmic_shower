set term pdf color
set out "output_plots/stats_relative.pdf"
datacsv="node.dat"
set datafile separator comma
set key off
set grid
set xlabel "t/min"
set ylabel "ratio"

f(x) = a*x + b
fit f(x) datacsv using 1:($3/$5) via a,b

set title sprintf("estimated runtime %fh", ((1 - b)/a/60))

plot datacsv using 1:($3/$5) with lines, f(x)



set out "output_plots/stats_energy.pdf"
set ylabel "E/MeV"

e(x) = c*x + d
fit e(x) datacsv using 1:7 via c,d

m(x) = 10**10

set title "Energy simulated"
set title sprintf("Energy simulated, estimated runtime %fh", ((10**10 - d)/c/60))

plot datacsv using 1:7 with lines, e(x), m(x)



set out "output_plots/stats_absolute.pdf"
set key top left outside
set ylabel "N"

set title "total and finished simulations"

plot datacsv using 1:5 title "total" with lines, datacsv using 1:3 title "finished" with lines



set out "output_plots/stats_current-failed.pdf"

set title "Total number of currently run and failed simulations"

plot datacsv using 1:2 title "current",  datacsv using 1:4 title "failed"
