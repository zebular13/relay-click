/**
 * app.js connects index.html and ledClick.js together.
 * Modified to say 'led' instead of relay
 * User activity is required for Web BLuetooth the make the connection
 */
let bleSwitch = document.querySelector('#bleSwitch');

/** All activity is conducted in the context of the annyang object.
 * voice recognition occurs on the web/cloud so must have an open
 * wifi connection for this to work.
 */
if (annyang){
 
  var ledOne = function(){
    console.log('led One');
    ledClick._writeCharacteristic(ledClick.characteristic1UUID, new Uint8Array([1]))
   .then(() => console.log('wrote 1'))
   .catch(error => {console.log('write error');
   });
  };
  
  var ledTwo = function(){
   console.log('led Two'); 
   ledClick._writeCharacteristic(ledClick.characteristic1UUID, new Uint8Array([2]))
   .then(() => console.log('wrote 2'))
   .catch(error => {console.log('write error');
   });
  };
  /**
   * Key value paris of voice commands, list can be expanded at will.
   */
  var commands = { 
    'led 1': ledOne,
    'led1': ledOne,
    'one': ledOne,
    '1': ledOne,
    'lamp': ledOne,
    'desklamp': ledOne,
    'desk lamp': ledOne,
    'led 2': ledTwo,
    'led2': ledTwo,
    'two': ledTwo,
    '2': ledTwo
  };
  
  // with annyang.debug below these call backs could be removed I think . . .
  annyang.addCallback('resultMatch',function(userSaid,commandText,phrases){
    console.log(userSaid);
    console.log(commandText);
    console.log(phrases);
  });
  
  annyang.addCallback('resultNoMatch',function(phrases){
    console.log('no match');
    console.log(phrases);
  });
  
  annyang.debug();
  annyang.addCommands(commands);
  annyang.setLanguage('en');
  annyang.start();
};

bleSwitch.addEventListener('click',function(){
  console.log('new switch click, connect');
  ledClick.connect()
      .then(() => console.log('connected'))
      .catch(error => { console.log('connect error!');
    });
});
