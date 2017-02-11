% clear; close all; clc;
% file = fopen('font_79.bmp');
% x = fread(file, 1, 'int');
% y = fread(file, 1, 'int');
% raw = fread(file);
% img_raw = reshape(raw(1:3:end), [x/3 y]);
% imagesc(img_raw, [0 255])
% colormap(gray)

close all;
armor_base = 0:300;
armor_bonus = 70;
armor_total = armor_base + armor_bonus;
armor_final = armor_base*0.7*0.93 + armor_bonus*0.7*0.93*0.55 - 74;
dmg = 4300*100./(100 + armor_final);
plot(armor_total, dmg)
grid