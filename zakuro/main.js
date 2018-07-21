const puppeteer = require('puppeteer');
const process = require('process')
const fs = require('fs')
const path = require('path')

if(process.argv.length == 2) {
  console.log('node main.js [model dir] [trace dir]')
}

let [modelDir, traceDir] = process.argv.slice(2);
let files = [];

fs.readdirSync(modelDir).filter(f => f.endsWith('.mdl')).forEach(file => {
  let num = parseInt(file.match(/\d+/)[0], 10);
  files[num] = { model: path.join(modelDir, file) }
});
fs.readdirSync(traceDir).filter(f => f.endsWith('.nbt')).forEach(file => {
  let num = parseInt(file.match(/\d+/)[0], 10);
  files[num].trace = path.join(traceDir, file);
});

console.log("Case,Time,Energy,ClockTime");

(async () => {
  const browser = await puppeteer.launch(/*{headless: false}*/);
  const page = await browser.newPage();
  await page.goto('https://icfpcontest2018.github.io/lgtn/exec-trace-novis.html');
  await page.evaluate(() => {
    let boost = document.querySelector('option[value="4000"]');
    boost.value = 100000;
    boost.selected = true;
  });
  let idx = 0;
  for(let file of files) {
    if(!file) continue;
    idx++;
    let {model, trace} = file;
    await (await page.$('#tgtModelFileIn')).uploadFile(model);
    await (await page.$('#traceFileIn')).uploadFile(trace);
    await (await page.$('#execTrace')).click();
    await page.waitForFunction('!document.getElementById("execTrace").disabled');
    let result = await (await (await page.$("#stdout")).getProperty('textContent')).jsonValue();
    if(result.startsWith('Success')) {
      let lines = result.split('\n');
      console.log([idx].concat(lines.slice(1,5).map(s => parseInt(s.match(/\d+/)[0], 10))).join(','));
    } else {
      console.error("Error!");
      console.error(result);
      await browser.close();
      process.exit(1);
    }
  }

  await browser.close();
})();