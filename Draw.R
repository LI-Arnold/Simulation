#pour executer le code : R CMD BATCH Draw.R
modelname = "Simulation_nb_conteneurs.data"
data = read.table(modelname)
attach(data); 


temps = V1
Nanneau = V2

plot(temps,Nanneau,type="l",xlab="temps", ylab="Quantité",col="red",main="Simulation nombre de slots occupé")
