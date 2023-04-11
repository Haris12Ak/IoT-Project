// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
    apiKey: "AIzaSyAtWqhMOGj84h4CWOLya80ZE_aCmEqvX6Q",
    authDomain: "upravljanje-klimom.firebaseapp.com",
    databaseURL: "https://upravljanje-klimom-default-rtdb.firebaseio.com",
    projectId: "upravljanje-klimom",
    storageBucket: "upravljanje-klimom.appspot.com",
    messagingSenderId: "617723074572",
    appId: "1:617723074572:web:a54a88713db580dd13b629",
    measurementId: "G-5FPVN59TMV"
};

// Initialize Firebase
firebase.initializeApp(firebaseConfig);
firebase.analytics();

var demo = document.getElementById("demo");
var demo1 = document.getElementById("demo_1");
var senzorPokreta = document.getElementById("senzor");
var senzorPokretaTxt = document.getElementById("senzorPokretaTxt");
var trenutnaTemp = document.getElementById("trenutnaTemp");
var optimalnaTemp = document.getElementById("optimalnaTemp");
var klima_dnevni = document.getElementById("klima_dnevni");
var senzorActive = document.getElementsByClassName("senzorActive");
var napomena = document.getElementById("napomena");
var slikaKlima = document.getElementById("slika");
var upaljena = document.getElementById("upaljena");
var upaljena_1 = document.getElementById("upaljena_1");
var ugasena = document.getElementById("ugasena");
var upali = document.getElementById("upali");
var ugasi = document.getElementById("ugasi");

var dbOptimalnaTemp = firebase.database().ref().child('Temperatura/optimalna');
var btnUkljuci_hladenje = document.getElementById("ukljuci_hladenje");
var btnUkljuci_grijanje = document.getElementById("ukljuci_grijanje");

var temp = null;

const optimalna = 221;

dbOptimalnaTemp.on('value', (podatak) => {
    optimalnaTemp.innerHTML = podatak.val();
});

firebase.database().ref('Temperatura').set({
    optimalna: optimalna
}, (error) => {
    if (error) {
        console.log("Greska u snimanju podataka!");
    } else {
        console.log("Optimalna temperatura uspjesno snimljena!");
    }
});