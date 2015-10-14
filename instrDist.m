cycles=139:1:164;
P=cycles-139+1;
delay=cycles*1/(20e6)

for i=1:26
    Pdist(i)=nchoosek(25,i-1)*(4/5)^(i-1)*(1/5)^(25-i+1);
    
end
sum(Pdist);
cdf(1)=0
for i=2:26
    cdf(i)=cdf(i-1)+Pdist(i-1)
end

plot(delay,cdf)
xlabel('Delay(s)')
ylabel('Cumulative Density')

%===================
% plot(delay,Pdist)
% xlabel('Delay(s)')
% ylabel('Probability')

max(delay)
min(delay)
max(delay)-min(delay)
0.9*(max(delay)-min(delay))

tint=83:93
intdelay=tint/20e6
for i=1:11
    Pint(i)=nchoosek(10,i-1)*(4/5)^(i-1)*(1/5)^(10-i+1);
end
max(intdelay)
min(intdelay)
% plot(cycles,Pdist)
% xlabel('Cycles')
% ylabel('Probability')
delay_inte=delay*1e9;
intdelay_inte=intdelay*1e9;

for i=1:26
    for j=1:11
       
        prob_matrix(i,j)=cycles(i)/(1e-3*20e6-tint(j))*Pdist(i)*Pint(j);
    end
end


