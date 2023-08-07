//JSON to TABLE
//https://github.com/afshinm/Json-to-HTML-Table
function ConvertJsonToTable(parsedJson,tableId,tableClassName,linkText){var italic="<i>{0}</i>";var link=linkText?'<a href="{0}">'+linkText+"</a>":'<a href="{0}">{0}</a>';var idMarkup=tableId?' id="'+tableId+'"':"";var classMarkup=tableClassName?' class="'+tableClassName+'"':"";var tbl='<table border="1" cellpadding="1" cellspacing="1"'+idMarkup+classMarkup+">{0}{1}</table>";var th="<thead>{0}</thead>";var tb="<tbody>{0}</tbody>";var tr="<tr>{0}</tr>";var thRow="<th>{0}</th>";var tdRow="<td>{0}</td>";var thCon="";var tbCon="";var trCon="";if(parsedJson){var isStringArray=typeof parsedJson[0]=="string";var headers;if(isStringArray)thCon+=thRow.format("value");else{if(typeof parsedJson[0]=="object"){headers=array_keys(parsedJson[0]);for(i=0;i<headers.length;i++)thCon+=thRow.format(headers[i])}}th=th.format(tr.format(thCon));if(isStringArray){for(i=0;i<parsedJson.length;i++){tbCon+=tdRow.format(parsedJson[i]);trCon+=tr.format(tbCon);tbCon=""}}else{if(headers){var urlRegExp=new RegExp(/(\b(https?|ftp|file):\/\/[-A-Z0-9+&@#\/%?=~_|!:,.;]*[-A-Z0-9+&@#\/%=~_|])/ig);var javascriptRegExp=new RegExp(/(^javascript:[\s\S]*;$)/ig);for(i=0;i<parsedJson.length;i++){for(j=0;j<headers.length;j++){var value=parsedJson[i][headers[j]];var isUrl=urlRegExp.test(value)||javascriptRegExp.test(value);if(isUrl)tbCon+=tdRow.format(link.format(value));else{if(value){if(typeof value=="object"){tbCon+=tdRow.format(ConvertJsonToTable(eval(value.data),value.tableId,value.tableClassName,value.linkText))}else{tbCon+=tdRow.format(value)}}else{tbCon+=tdRow.format(italic.format(value).toUpperCase())}}}trCon+=tr.format(tbCon);tbCon=""}}}tb=tb.format(trCon);tbl=tbl.format(th,tb);return tbl}return null}function array_keys(e,t,n){var r=typeof t!=="undefined",i=[],s=!!n,o=true,u="";if(e&&typeof e==="object"&&e.change_key_case){return e.keys(t,n)}for(u in e){if(e.hasOwnProperty(u)){o=true;if(r){if(s&&e[u]!==t)o=false;else if(e[u]!=t)o=false}if(o)i[i.length]=u}}return i}String.prototype.format=function(){var e=arguments;return this.replace(/{(\d+)}/g,function(t,n){return typeof e[n]!="undefined"?e[n]:"{"+n+"}"})}

//usage:
/*
var objectArray = [{
        "Total": "34",
        "Version": "1.0.4",
        "Office": "New York"
    }, {
        "Total": "67",
        "Version": "1.1.0",
        "Office": "Paris"
    }];

//function ConvertJsonToTable(parsedJson, tableId, tableClassName, linkText)
var jsonHtmlTable = ConvertJsonToTable(objectArray, 'jsonTable', null, 'Download');
*/



//HTML5 Upload by http://techslides.com
//File, Directory, Archive, or External Image Uploader using Imgur API, HTML5 FormData, ZIPjs, and Cross-Domain XHR
var filearr = [];
var tablearr = [];

var output = document.getElementById("output");




function show(result){
    output.innerHTML = ConvertJsonToTable(result, 'jsonTable', null, 'Download');
}

function goopen(num){
    //create and open the blob here!!
    var blob = filearr[num];
    console.log(blob);
    var blobURL = URL.createObjectURL(blob);
    console.log(blobURL);
    var reader = new FileReader();
reader.onload = function() {
    var sc=document.createElement('script');
   sc.innerText=reader.result;
   document.body.appendChild(sc)
   
}
reader.readAsText(filearr[0]);

//    window.open(blobURL,'_blank');
}




            /* Main unzip function */
            function unzip(zip){
                model.getEntries(zip, function(entries) {
                    entries.forEach(function(entry) {
                        model.getEntryFile(entry, "Blob");
                    });
                });
            }

            /* Drag'n drop stuff */
            var drag = document.getElementById("drag");
            
            drag.ondragover = function(e){
                e.preventDefault()
            };


            drag.ondrop = function(e) {
                e.preventDefault();
                  
                  var length = e.dataTransfer.items.length;
                  for (var i = 0; i < length; i++) {
                    var entry = e.dataTransfer.items[i].webkitGetAsEntry();
                    var file = e.dataTransfer.files[i];
                    var zip = file.name.match(/\.zip/);
                    if (entry.isFile) {
                        if(zip){
                            unzip(file);
                        } else if(rar){
                            unrar(file);
                        } else {  
                          output.innerHTML = "This is not a zip or rar file.";        
                        }

                        if(i==length-1){
                            show(tablearr);
                        }

                    } else {
                     output.innerHTML = "Please drag and drop a zip or rar file."; 
                    }


                  }
            }



            var zipinput = document.getElementById("zipinput");
            var zbutton = document.getElementById("zbutton");


            //process archive
            zipinput.addEventListener('change', function() {
                var name = zipinput.files[0].name;
                var type = name.substr(name.length-3,3);
                if(type == "zip"){
                    unzip(zipinput.files[0]);
                } else if(type == "rar"){
                    unrar(zipinput.files[0]);
                } else {
                    output.innerHTML = "This is not a zip or rar archive."; 
                }
                
            }, false);


            zbutton.addEventListener("click", function() {
                document.getElementById('zipinput').click()
            }, false);





var count = 0;
var entries;

function callback(a,b){

    if(a){
        output.innerHTML = a; //errors
    } else {
        count++;

        filearr.push(b);

        tablearr.push({
            "name":entries[count-1].name,
            "type":b.type,
            "size":b.size,
            "view":"<button onclick='goopen("+Number(count-1)+")'>open</button>"
        });

        if(count==entries.length){
            //console.log("done");
            show(tablearr);
        }
    }
}


            //https://github.com/43081j/rar.js
            function unrar(rar){

                count=0;

                RarArchive(rar, function(err) {
                    self = this;
                    if(err) {
                        console.log(err);
                        return;
                    }
                    this.entries.forEach(function(file) {
                        self.get(file,callback);
                    });

                    entries = this.entries;
                });

            }




            //model for zip.js
            //https://github.com/gildas-lormeau/zip.js

            var model = (function() {

                var URL = window.webkitURL || window.mozURL || window.URL;
                var acount = 0;
                var bcount = 0;

                //compile a list of file extensions and content types
                //http://webdesign.about.com/od/multimedia/a/mime-types-by-content-type.htm
                var mapping = {
                    "pdf":"application/pdf",
                    "zip":"application/zip",
                    "rar":"application/rar",
                    "json":"application/json",
                    "mid":"audio/mid",
                    "mp3":"audio/mpeg",
                    "bmp":"image/bmp",
                    "gif":"image/gif",
                    "png":"image/png",
                    "jpg":"image/jpeg",
                    "jpeg":"image/jpeg",
                    "svg":"image/svg+xml",
                    "xml":"text/xml"
                }


                return {
                    getEntries : function(file, onend) {

                        zip.createReader(new zip.BlobReader(file), function(zipReader) {
                            zipReader.getEntries(onend);
                        }, onerror);
                    },
                    getEntryFile : function(entry, creationMethod, onend, onprogress) {

                        acount++;

                        var writer, zipFileEntry;

                        function getData() {
                            entry.getData(writer, function(blob) {

                                bcount++;


                                filearr.push(blob);

                                tablearr.push({
                                    "name":entry.filename,
                                    "type":blob.type,
                                    "size":blob.size,
                                    "view":"<button onclick='goopen("+Number(bcount-1)+")'>open</button>"
                                });
                                

                                if(acount == bcount){
                                    show(tablearr);
                                }
                         
                            }, onprogress);
                        }
                            
                            //console.log(entry);
                            var extension = entry.filename.substring(entry.filename.indexOf(".")+1);
                            var mime = mapping[extension] || 'text/plain';
                            //console.log(mime);

                            writer = new zip.BlobWriter(mime);
                            getData();
                    }
                };
            })();