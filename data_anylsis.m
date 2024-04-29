close all;
num_of_samples = 10; 
file_name = "";
fft_size = 1024;
spec_size = fft_size/2 +1;
fs = 125e6;
psd_per_sample = zeros(num_of_samples, spec_size);

for i=0:num_of_samples-1
    % load data:
    file_name = "signal_" + i + "_ch0.txt";
    loaded_sig = importdata(file_name, "\n");
    loaded_sig_un = unwrap(loaded_sig);
    [spec_i, f_vec] = pwelch(loaded_sig_un, [], [], fft_size, fs);
    psd_per_sample(i+1, :) = spec_i';
end

psd =(1/num_of_samples).* sum(psd_per_sample, 1);
figure();
plot(f_vec, 10*log10(psd));
title("spectrum from dut ")
ylabel("dB/Hz");
xlabel("frequncy [Hz]")

figure();
semilogx(f_vec, 10*log10(psd))
title("spectrum from dut - log scale ")
ylabel("dB/Hz");
xlabel("frequncy [Hz]")

%%
f = 1e6;
psd_per_sample_sim = zeros(num_of_samples, spec_size);
for i=0:num_of_samples-1
    % load data:
    file_name = "signal_" + i + "_ch1.txt";
    loaded_sig = importdata(file_name, "\n");
    x_n = loaded_sig';
    Q = cos(2*pi*f*t_norm+1*pi);
    I = sin(2*pi*f*t_norm+1*pi);

    % IQ modulation
    I_comp = x_n.*I;
    Q_comp = x_n.*Q;

    % insert the filter parameters
    [h,w] = freqz(Num6);
    [h1,t1] = impz(Num6);

    I_comp_fil = conv(I_comp,h1,"same");
    Q_comp_fil = conv(Q_comp,h1,"same");

    phase_from_sim = atan2(I_comp_fil ,Q_comp_fil);

    loaded_sig_un = unwrap(phase_from_sim);
    [spec_i, f_vec] = pwelch(loaded_sig_un, [], [], fft_size, fs);
    psd_per_sample_sim(i+1, :) = spec_i';
end

psd_sim =(1/num_of_samples).* sum(psd_per_sample_sim, 1);
figure();
plot(f_vec, 10*log10(psd_sim));
title("spectrum from dut | sim ")
ylabel("dB/Hz");
xlabel("frequncy [Hz]")

figure();
semilogx(f_vec, 10*log10(psd_sim))
%loglog(f, 10*log(psd_sim))
title("spectrum from dut | sim - log scale ")
ylabel("dB/Hz");
xlabel("frequncy [Hz]")



