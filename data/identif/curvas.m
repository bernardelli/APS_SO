close all

%%Constantes

To = 1/30; %tempode frame


%% SUBINDO

%tempo de interesse
t1s = 0; %s
t2s = 7;
altura_raws = csvread('2017-07-04-140914.csv');

ts = To*(0: (length(altura_raws)-1));


%aplicar janela
t_js = ts( ts > t1s & ts < t2s);
altura_js = altura_raws( ts > t1s & ts < t2s);

t_ds = 1.2; %tempo de degrau

us = zeros(length(t_js));

us(t_js<t_ds) = 4.54;

us(t_js>=t_ds) = 4.70;

step_value_s = (4.70 - 4.54)/12;


figure
hold on
yyaxis left
plot (t_js, altura_js);

yyaxis right
plot (t_js, us, '--');
title('subida');

legend('altura', 'entrada (V)');

%% DESCENDO

t1d = 1; %s
t2d = 4;
altura_rawd = csvread('2017-07-04-140620.csv');

td = To*(0: (length(altura_rawd)-1));


%aplicar janela
t_jd = td( td > t1d & td < t2d);
altura_jd = altura_rawd( td > t1d & td < t2d);

t_dd = 1.2; %tempo de degrau

ud = zeros(length(t_jd));

ud(t_jd<t_dd) = 4.62;

ud(t_jd>=t_dd) = 4.18;


step_value_d = (4.18 - 4.62)/12;

figure
hold on
yyaxis left
plot (t_jd, altura_jd);

yyaxis right
plot (t_jd, ud, '--');
title('descida');
legend('altura', 'entrada (V)');


%%PI

disp('PI prático');

Kp_p = 25/1000
Ki_p = (350/60000) /1000

%subindo

Ke_s = ((830-220)/1024)/step_value_s;

Tu_s = 1.567-t_ds;
Tg_s = 3.467-1.567;

Kp_s = 0.9*Tg_s/((Tu_s + To/2)*Ke_s) - 0.135*Tg_s*To/((Tu_s + To/2)*(Tu_s + To/2)*Ke_s);
Ki_s = 0.27*Tg_s*To/(Ke_s*(Tu_s + To/2)*(Tu_s + To/2));

disp('PI subindo');
Kp_s
Ki_s

%descendo

Ke_d = ((35-645)/1024)/step_value_d;

Tu_d = 1.5-t_dd;
Tg_d = 2.93-1.5;

Kp_d = 0.9*Tg_d/((Tu_d + To/2)*Ke_d) - 0.135*Tg_d*To/((Tu_d + To/2)*(Tu_d + To/2)*Ke_d);
Ki_d = 0.27*Tg_d*To/(Ke_d*(Tu_d + To/2)*(Tu_d + To/2));

disp('PI descendo');
Kp_d
Ki_d




