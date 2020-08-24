% Feedhelper 400
% Initial data analysis

close all;

% Jännite
U = [
    3.4
    4
    4.4
    4.8
    5.1
    5.2
    5.3
    ];

% Langansyöttö
F = [
    6
    7
    9
    12
    13
    14
    15
    ];

figure;
hold on;
scatter(U, F);
axis([0 6 0 20]);

ft = fittype('a*x^2 + b*x + c');
fit1 = fit(U, F, ft);

A = fit1.a;
B = fit1.b;
C = fit1.c;

syms x
u_f(x) = A*x^2 + B*x + C;

diff_u_f = diff(u_f);
fit_min = solve(diff_u_f == 0, x);
fplot(u_f);





