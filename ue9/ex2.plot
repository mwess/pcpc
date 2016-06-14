# Needs following input in sailor_data.txt :
# TIME	POSITION_SAILOR_1	POSITION_SAILOR_2	POSITION_SAILOR_3	POSITION_SAILOR_4

# run "gnuplot sailor.plot" and you will get "sailors_plot.pdf"

set term pdf


#set output "sailors_plot.pdf"
#plot  "sailor_data.txt" u 2 w lp not, "sailor_data.txt" u 3 w lp not, "sailor_data.txt" u 4 w lp not, "sailor_data.txt" u 5 w lp not, 0.5 ls 0 not , 51.5 ls 0 not

set output "ex2_plot.pdf"

plot  "ex2_data" u 2 w lp not, "ex2_data" u 3 w lp not, "ex2_data" u 4 w lp not, "ex2_data" u 5 w lp not, 0.5 ls 0 not , 51.5 ls 0 not
