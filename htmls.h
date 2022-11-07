const char PASSCHANGE_PAGE[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <title>Bootstrap Example</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        .form-control {
            display: block;
            width: 100%;
            height: calc(1.5em + .75rem + 2px);
            padding: .375rem .75rem;
            font-size: 1rem;
            font-weight: 400;
            line-height: 1.5;
            color: #495057;
            background-color: #fff;
            background-clip: padding-box;
            border: 1px solid #ced4da;
            border-radius: .25rem;
            transition: border-color .15s ease-in-out, box-shadow .15s ease-in-out;
        }

        .row {

            display: -webkit-box;
            display: -ms-flexbox;
            display: flex;
            -ms-flex-wrap: wrap;
            flex-wrap: wrap;
            margin-right: -15px;
            margin-left: -15px;
        }

        .col-lg-4 {
            position: relative;
            width: 100%;
            padding-right: 15px;
            padding-left: 15px;
        }

        .btn {
            display: inline-block;
            font-weight: 400;
            color: #212529;
            text-align: center;
            vertical-align: middle;
            -webkit-user-select: none;
            -moz-user-select: none;
            -ms-user-select: none;
            user-select: none;
            background-color: transparent;
            border: 1px solid transparent;
            padding: .375rem .75rem;
            font-size: 1rem;
            line-height: 1.5;
            border-radius: .25rem;
            transition: color .15s ease-in-out, background-color .15s ease-in-out, border-color .15s ease-in-out, box-shadow .15s ease-in-out;
        }

        .btn-outline-success {
            color: #28a745;
            background-color: transparent;
            background-image: none;
            border-color: #28a745;
        }

        .container {
            width: 80%;
            padding-right: 15px;
            padding-left: 15px;
            margin-right: auto;
            margin-left: auto;
        }
    </style>
</head>
<body>

<h1>Ustaw Wifi</h1>
<form action="/processpass">
    <div class="container">
        <div class="row">
            <div class="col-lg-4">
                <label for="">SSID</label>
                <input name="ssid" type="text" class="form-control">
            </div>
            <div class="col-lg-4">
                <label for="">Password</label>
                <input name="password" type="text" class="form-control">
            </div>
             <div class="col-lg-4">
                <label for="">HostName</label>
                <input name="hostname" type="text" class="form-control">
            </div>
            <div class="col-lg-4">
                <button type="submit" class="btn btn-outline-success">Zapisz</button>
            </div>
        </div>
    </div>
</form>
</body>
</html>
)=====";

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="pl">
<head>
    <title>BM280 Pomiary </title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.6.1/dist/css/bootstrap.min.css">
    <script src="https://cdn.jsdelivr.net/npm/jquery@3.6.0/dist/jquery.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/popper.js@1.16.1/dist/umd/popper.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@4.6.1/dist/js/bootstrap.bundle.min.js"></script>
</head>
<body>

<div class="container">
    <div class="row">
        <div class="col-lg-12 blockkolor">
            <div class="jumbotron">
                <h1 id="Temp"></h1>
            </div>
        </div>
        <div class="col-lg-12 blockkolor">
            <div class="jumbotron">
                <h1 id="human"></h1>
            </div>
        </div>
        <div class="col-lg-12 blockkolor">
            <div class="jumbotron">
                <h1 id="humanAbsolute"></h1>
            </div>
        </div>
        <div class="col-lg-12 blockkolor">
            <div class="jumbotron">
                <h1 id="punktrosy"></h1>
            </div>
        </div>
        <div class="col-lg-12 blockkolor">
            <div class="jumbotron">
                <h1 id="pressure"></h1>
            </div>
        </div>
    </div>
</div>
<script>
    var h=0;
    var b=0;
    function getDataFromESP(){
        $.post('/temp',function (result){
            result=result.trim();
            $('#Temp').html('Temperatura '+result+' °C');
            try{
                b=parseFloat(result);
            }
            catch (e){

            }
        }).fail(()=>{$('#Temp').html('Temperatura  Brak Odczytu');});
        $.post('/human',function (result){
            result=result.trim();
            $('#human').html('Wilgotność '+result+' %');
            try{
                h=parseFloat(result);
                //(6,112*2,71828^((17,67*B9)/(B9+243,5))*C9*2,1674)/(273,15+B9)
                var humanAbsolute=(6.112*Math.pow(2.71828,(17.67*b)/(b+243.5))*h*2.1674)/ (273.15+b);
                humanAbsolute=Math.round(humanAbsolute*100)/100;
                $('#humanAbsolute').html('Wilgotność absolutna '+humanAbsolute+' G/m3');

                var punktRosy=Math.round((243.04*(Math.log(h/100)+(17.625*b/(243.04+b)))/(17.625-Math.log(h/100)-(17.625*b/(243.04+b))))*100)/100;
                $('#punktrosy').html('Punkt Rosy '+punktRosy+'°C');

            }
            catch (e){
                
            }
        }).fail(()=>{$('#human').html('Wilgotność  Brak Odczytu');});
        $.post('/pressure',function (result){
            result=result.trim();
            $('#pressure').html('Ciśnienie '+result+' kPa');
            setTimeout(getDataFromESP,1000);
        }).fail(()=>{$('#pressure').html('Ciśnienie  Brak Odczytu'); setTimeout(getDataFromESP,1000);});



    }
    $(()=>{
        getDataFromESP();
    });
</script>
</body>
</html>
)=====";
