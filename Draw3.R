#pour executer le code : R CMD BATCH Draw.R
modelname = "Simulation_attente10.data"
data = read.table(modelname)
attach(data); 


temps = V1
attente = V2

pdf(file="Simulation_délai_attente_station10.pdf")
plot(temps,attente,type="l",xlab="temps", ylab="temps attente",col="red",main="Simulation délai d'attente station 10")
