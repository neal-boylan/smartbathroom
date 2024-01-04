// Your web app's Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyCRu-PyVZ9AaYLGuEiJVwcGNceIYp80dm0",
    authDomain: "smart-bathroom-6bd1b.firebaseapp.com",
    databaseURL: "https://smart-bathroom-6bd1b-default-rtdb.europe-west1.firebasedatabase.app",
    projectId: "smart-bathroom-6bd1b",
    storageBucket: "smart-bathroom-6bd1b.appspot.com",
    messagingSenderId: "815200862494",
    appId: "1:815200862494:web:2a9072b430c5b0a31c090f"
};

firebase.initializeApp(firebaseConfig);

// Get a reference to the database service
const database = firebase.database();

// Create database reference
const camRef = database.ref("events/button");

// Sync on any updates to the DB. THIS CODE RUNS EVERY TIME AN UPDATE OCCURS ON THE DB.
camRef.limitToLast(1).on("value", function(snapshot) {
  snapshot.forEach(function(childSnapshot) {
    const event = childSnapshot.val()["event"];
    const temp = childSnapshot.val()["temperature"];
    const tempSet = childSnapshot.val()["tempSet"];
    const humidity = childSnapshot.val()["humidity"];
    const humiditySet = childSnapshot.val()["humiditySet"];
    const count = childSnapshot.val()["count"];
    const timeStamp = childSnapshot.val()["timeStamp"];
    const date = new Date();
    let d = String(date.getDate()).padStart(2, '0');
    let m = String((date.getMonth() + 1)).padStart(2, '0');
    let y = String(date.getFullYear());
    let hr = String(date.getHours()).padStart(2, '0');
    let min = String(date.getMinutes()).padStart(2, '0');
    let sec = String(date.getSeconds()).padStart(2, '0');
    
    const timeDate = hr + ":" + min + ":" + sec + ", " + d + "/" + m + "/" + y;

    document.getElementById("events").innerText += `Date: ${timeDate}, Event: ${event}, Temp: ${temp}, Temp Setpoint: ${tempSet}, Humidity: ${humidity}, Humidity Setpoint: ${humiditySet}\n`
      })
  });

