import { httpRequest,redraw} from "../jsrootsys/jsrootsys/modules/main.mjs";
/*  var mod = await import("../jsrootsys/jsrootsys/modules/main.mjs");
    alert("awaited")
                    console.log('awaited');
 var { httpRequest, redraw } = mod;
                    console.log("httpRequest "+ httpRequest);
    alert("httpRequest "+ httpRequest)
if (!httpRequest) {
    alert("unavailable!")
                    console.log("unavailable!");
   var { httpRequest, redraw } = await import("../root.dst/jsrootsys/modules/main.mjs");
} */

async function FillRefSelector(path, selID) {
    let obj = await httpRequest(path, 'object');
        var refList = JSON.parse(obj);
        var sel = document.getElementById(selID);
        sel.innerHTML = "";
        refList.arr.forEach(function (item) {
            var op = document.createElement("option");
            op.value = item.fString;
            op.innerHTML = item.fString;
            sel.appendChild(op);
        });
}

async function FillRefTable(path, selID) {
    var sel = document.getElementById(selID);
    if ((sel.size == 0) || (sel.innerHTML == "")){
    let obj = await httpRequest(path, 'object');
            var refList = obj;//JSON.parse(obj);
            var refLen = refList.arr.length;
            var runId = 0;
            var storedLen = sessionStorage.getItem('refLen');
            //alert('stored ' + storedLen + '  reflen '+ refLen);
            if (refLen == storedLen)
                return;
            sel.innerHTML = "";
            refList.arr.forEach(function (item) {
                var op = document.createElement("option");
                var s = 'Run ' + item.i_run_number + ', beam ' + item.str_beam_particle + ', energy ' + item.d_energy;
                s += ',  target ' + item.str_target_particle;
                if (item.d_field_voltage > -1)
                    s += ', Voltage ' + item.d_field_voltage;
                op.innerHTML = s;
                op.value = item.i_run_number;
                sel.appendChild(op);
            });
            var id = sel.value;
            var refInput = document.getElementById("refInput");
            //refInput.value = id;
//            sessionStorage.setItem('runID', item.i_run_number);
            sessionStorage.setItem('refLen', refLen);
    }
}

function makeSelection(ref_list_name = "refTable", ref_input_name = "refInput") {
    var rt = document.getElementById(ref_list_name);
    var id = rt.value;
           // alert('selected id ' + id);
    var refInput = document.getElementById(ref_input_name);
    if (rt.innerHTML == "")
        return;
    refInput.value = id;
}

async function setRunInfo(path) {
    let obj = await httpRequest(path, 'object');
        var info = obj;//JSON.parse(obj);
        if (info.arr.length > 0){
            var runInfo = info.arr[0];
//            alert('i_run_number '+runInfo.i_run_number);
            sessionStorage.setItem("runID", runInfo.i_run_number);
        }
}

function createSelectorsTable(selID) {
    var div = document.getElementById(selID);
    var table = div.create("table");
    table.borderWidth = 1;

}

async function Redraw2Div(addr, divName, opt = "") {
    var obj = await httpRequest(addr, 'object');
    redraw(divName, obj, opt);
}
                
export {FillRefSelector, FillRefTable, makeSelection, setRunInfo, createSelectorsTable,
    Redraw2Div};

