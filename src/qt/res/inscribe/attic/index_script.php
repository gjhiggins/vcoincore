<?php
require_once "config.inc.php";

if( array_key_exists("q", $_REQUEST) && $_REQUEST["q"] == "sendTx" ){

  $body = file_get_contents('php://input');   
  
  $ch = curl_init();
  
  curl_setopt($ch, CURLOPT_USERPWD, CHAIN_ID . ":" . CHAIN_SECRET);  
  curl_setopt($ch, CURLOPT_URL, "https://api.chain.com/v2/bitcoin/transactions/send");
  curl_setopt($ch, CURLOPT_POST, true); 
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_POSTFIELDS, $body);   
  $output = curl_exec($ch); 
  
  echo $output;
  exit(0);  
}

?>

<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Bitcoin: Create transactions</title>

    <link href='http://fonts.googleapis.com/css?family=Open+Sans:400,700,800' rel='stylesheet' type='text/css'>
    <link href='http://fonts.googleapis.com/css?family=Lato:400,700,900' rel='stylesheet' type='text/css'>
    
    <link href="bootstrap/css/bootstrap.min.css" rel="stylesheet">
    <link href="setfive.css" rel="stylesheet">
    <link href="style.css" rel="stylesheet">
        
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>
    <script src="bootstrap/js/bootstrap.min.js"></script>
    <script src="underscore-min.js"></script>    
    <script src="bitcoinjs.js"></script>    
    
    <!-- HTML5 shim and Respond.js for IE8 support of HTML5 elements and media queries -->
    <!-- WARNING: Respond.js doesn't work if you view the page via file:// -->
    <!--[if lt IE 9]>
      <script src="https://oss.maxcdn.com/html5shiv/3.7.2/html5shiv.min.js"></script>
      <script src="https://oss.maxcdn.com/respond/1.4.2/respond.min.js"></script>
    <![endif]-->
  </head>
  <body>
    
<a href="http://symf.setfive.com/btcscript/"><img style="position: absolute; top: 0; right: 0; border: 0;" 
src="https://camo.githubusercontent.com/365986a132ccd6a44c23a9169022c0b5c890c387/68747470733a2f2f73332e616d617a6f6e6177732e636f6d2f6769746875622f726962626f6e732f666f726b6d655f72696768745f7265645f6161303030302e706e67" alt="Fork me on GitHub" 
data-canonical-src="https://s3.amazonaws.com/github/ribbons/forkme_right_red_aa0000.png"></a>    
    
    <div role="navigation" class="navbar navbar-inverse navbar-fixed-top">
          <div class="container">
              <div class="navbar-header">
                  <button data-target=".navbar-collapse" data-toggle="collapse" class="navbar-toggle" type="button">
                      <span class="sr-only">Toggle navigation</span>
                      <span class="icon-bar"></span>
                      <span class="icon-bar"></span>
                      <span class="icon-bar"></span>
                  </button>
                  <a href="#" class="navbar-brand"></a>
              </div>
              <div class="collapse navbar-collapse">
                  <div class="logo">
                      <a href="http://www.setfive.com"><img src="http://www.setfive.com/images/logo_noconsulting.png"></a>
                  </div>                  
              </div><!--/.nav-collapse -->
          </div>
      </div>    
    
    <div class="body-container casestudy-body-container">
      <div class="container">
        <div class="row">        
          <div class="col-md-8 col-md-offset-2">
                 
            <div class="section">
              <h2 class="section-title"><span>G</span>et Keys</h2>
              <form data-provide="generate-keys">
                <div class="form-group">
                  <label for="privKey">Private Key</label>
                  <input type="text" class="form-control" id="privKey" placeholder="Private Key">
                </div>
                
                <div class="form-group">
                  <label for="sendAddress">Public Address</label>
                  <input type="text" class="form-control" id="sendAddress" placeholder="Send To Address">
                </div>
                                
                <ul class="list-unstyled">
                  <li class="pull-left"><button data-provide="import" type="button" class="btn btn-default">Import Key</button></li>
                  <li class="pull-right"><button type="submit" class="btn btn-primary">Generate keys</button></li>                  
                </ul>
                
                <div class="clearfix"></div>
                
              </form>
            </div>
            
            <div class="section">
              <h2 class="section-title"><span>C</span>heck Address</h2>
              <p>You'll need to make sure you a high enough balance to actually send anything (plus fees). 
                 Click "Refresh" below to grab the info for the keys above.</p>
              
              <div data-provide="balance-container"></div>              
              
            </div>
            
            <div class="section">
              <h2 class="section-title"><span>C</span>reate A Transaction</h2>            
              <form data-provide="create-tx">
                
                <div class="form-group">
                  <label for="inputTx">Input Hash</label>
                  <ul class="list-unstyled list-inline">
                    <li><input type="text" class="form-control" id="inputTx" name="inputTx" placeholder="Input Hash" required></li>
                    <li><input type="text" class="form-control" id="inputIndex" name="inputIndex" placeholder="Index" required></li>
                  </ul>
                </div>              
  
                <div class="form-group">
                  <label for="destAddr">Destination Address</label>
                  <input type="text" class="form-control" name="destAddr" id="destAddr" placeholder="Destination Address" required>
                </div>
                
                <div class="form-group">
                  <label for="sendAmnt">Send Amount (Satoshi)</label>
                  <input type="text" class="form-control" name="sendAmnt" id="sendAmnt" placeholder="Send Amount" required>
                </div>              
                
                <div class="form-group">
                  <label for="noOpData">OP_RETURN Data</label>
                  <textarea name="noOpData" id="noOpData" class="form-control"></textarea>
                  <p>If set, no BTC will be sent to the destination. Only a OP_RETURN output will be created with this data. 
                    <a href="http://bitzuma.com/posts/op-return-and-the-future-of-bitcoin/" target="_blank">
                      More Info about OP_RETURN
                    </a>
                </div>                
                
                <div class="form-group">
                  <label for="btcScript">Script</label>
                  <textarea name="btcScript" id="btcScript" class="form-control"></textarea>
                </div>
                
                <ul class="list-inline list-unstyled">
                  <li><button type="reset" class="btn btn-danger">Clear</button></li>
                  <li><button type="submit" class="btn btn-primary">Send Transaction</button></li>
                  <li><img src="loader.gif" class="hidden" data-provide="send-loader" /></li>
                </ul>
              </form>
              
              <pre class="alert alert-info hidden" data-provide="send-info"></pre>
              
          </div>                    
          
        </div>
      </div>
    </div>

  <script type="text/template" id="balanceTemplate">
     
     <table class="table table-bordered table-striped">
      <tr>
        <td colspan="4">
          <a class="btn btn-primary" href="#" data-provide="refresh-balance">Refresh</a>
          <img src="loader.gif" class="hidden" data-provide="loader" />
        </td>
      </tr>     
      <tr>
        <th>Type</th>
        <th>Balance</th>
        <th>Received</th>
        <th>Sent</th>
      </tr>
      <tr>
        <td>Confirmed</td>   
        <td><%= confirmed.balance %></td>
        <td><%= confirmed.received %></td>
        <td><%= confirmed.sent %></td>               
      </tr>
      <tr>
        <td>Total (Confirmed+Unconfirmed)</td>
        <td><%= total.balance %></td>
        <td><%= total.received %></td>
        <td><%= total.sent %></td>                                    
      </tr>
    </table>
    
    <table class="table table-bordered table-striped">
      <tr>
        <th>Unspent (Click to use in TX)</th>
        <th>Amount</th>
      </tr>      
      <% _.each(unspents, function(tx){ %>
        <tr>          
          <td><a href="#<%= tx.transaction_hash %>" data-index="<%= tx.output_index %>" 
               data-provide="use-tx"><%= tx.transaction_hash.substr(0, 30) %>...</a></td>
          <td><%= tx.value %></td>
        </tr>
      <% }); %>
    </table>
    
    <table class="table table-bordered table-striped">
      <tr>
        <th>Transaction ID</th>
        <th>Confirmations</th>        
      </tr>
      <% _.each(transactions, function(tx){ %>
        <tr>
          <td><a href="#<%= tx.hash %>" data-provide="view-tx-info"><%= tx.hash.substr(0, 30) %>...</a></td>
          <td><%= tx.confirmations %></td>
        </tr>
      <% }); %>
    </table>
    
    <pre class="hidden" data-provide="tx-info"></pre>
    
  </script>

  <script>
    
    var CURRENT_NETWORK = bitcoin.networks.bitcoin;
        
    var Chain = {
      
      baseUrl: "https://api.chain.com/v2/bitcoin/",
      key: "35eb3c2a1b068461f9f01917d942fa0f",
      
      getAddressInfo: function(address){
        var balanceDf = Chain.makeRequest("addresses/" + address);        
        var unspentDf = Chain.makeRequest("addresses/" + address + "/unspents");
        var transactionDf = Chain.makeRequest("addresses/" + address + "/transactions");        
        var df =  $.Deferred();
        
        $.when(balanceDf, unspentDf, transactionDf).done(function(balancesResult, unspentResult, transactionsResult){        
          var addrInfo = _.extend({}, {unspents: unspentResult[0]}, {transactions: transactionsResult[0]}, balancesResult[0][0]);
          df.resolve(addrInfo);                                            
        }); 
        
        return df;
      },
      
      sendTransaction: function(body){
        return $.post("index.php?q=sendTx", JSON.stringify({"signed_hex": body}));
      },
      
      makeRequest: function(urlFragment, params){
        params = params ? params : {};
        var data = $.extend( {}, {"api-key-id": Chain.key}, params );
        var url = Chain.baseUrl + urlFragment;
        
        return $.ajax({
          url: url,
          data: data,
          type: 'GET'          
        });       
      }
      
    };
        
    $(document).ready(function(){
            
      var template = _.template( $("#balanceTemplate").text() );
      var emptyObj = {"balance": "", "received": "", "sent": ""};
      
      $("[data-provide='balance-container']").html( template( {confirmed: emptyObj, total: emptyObj, unspents: [], transactions: []} ) );
      
      $("[data-provide='create-tx']").submit(function(){
                               
        var privKey = $("#privKey").val();
        var currAddr = $("#sendAddress").val();
        var key = bitcoin.ECKey.fromWIF(privKey);        
        
        var form = $(this).serializeArray();        
        
        var input = _.findWhere(form, {name: "inputTx"}).value;
        var inputIndex = _.findWhere(form, {name: "inputIndex"}).value;
        
        var output = _.findWhere(form, {name: "destAddr"}).value;
        var outputAmnt = _.findWhere(form, {name: "sendAmnt"}).value;
        var noOpData = _.findWhere(form, {name: "noOpData"}).value;
        var script = _.findWhere(form, {name: "btcScript"}).value;
        
        var blanks = _.reduce([input, inputIndex, output, outputAmnt], 
                        function(numBlank, val){ return val.length > 0 ? numBlank : numBlank + 1; }, 0);
        
        if( blanks > 0 ){
          alert("You must specify all values.");
          return false;
        }                        
        
        outputAmnt = parseInt(outputAmnt);
        inputIndex = parseInt(inputIndex);
        
        $("[data-provide='send-loader']").removeClass("hidden");
        Chain.getAddressInfo(currAddr).done(function(addrInfo){
                  
          var hexTx = null;
          var nullScript = null;
          var myChange = addrInfo.confirmed.balance - outputAmnt - 1000;
          
          if( noOpData.length ){
            nullScript = bitcoin.scripts.nullDataOutput( new Buffer(noOpData) );
            myChange = addrInfo.confirmed.balance - 1000;
          }
            
          try {
            
            var tx = new bitcoin.TransactionBuilder();
                                
            tx.addInput(input, inputIndex);
            tx.addOutput(currAddr, myChange);
            
            if(nullScript){
              tx.addOutput(nullScript, 0);
            }                        
                        
            if(output.length){
              tx.addOutput(output, outputAmnt);
            }
            
            if(script.length){
              var btcScript = bitcoin.Script.fromASM(script);
              // tx.addOutput(btcScript, outputAmnt);
            }            
            
            tx.sign(0, key);            
            hexTx = tx.build().toHex();
            
          }catch(e){
            $("[data-provide='send-loader']").addClass("hidden");
            $("[data-provide='send-info']").html( "ERROR: " + e.message ).removeClass("hidden");
            return false;
          }                    
          
          Chain.sendTransaction(hexTx).done(function(data){
          
            $("#inputTx").val("");    
            $("[data-provide='send-loader']").addClass("hidden");
            
            var json = data;
            try {
              json = JSON.stringify(JSON.parse(data), null, "  ");
            }catch(e){
              json = "ERROR: " + e.message;
            }
            
            $("[data-provide='send-info']").html(json).removeClass("hidden");
          });
                  
        });        
                        
        return false;
      });
      
      $("[data-provide='send-tx']").submit(function(){
        var txBody = $("#txBody").val();
        
        if( txBody.length == 0 ){
          alert("Sorry! You can't send an empty transaction.");
          return false;
        }        
        
        return false;
      });
      
      $("[data-provide='create-tx']").on("reset", function(){
        $(this).find("input, textarea").val("");
        return false;
      });            
      
      $("body").on("click", "[data-provide='use-tx']", function(){
        var txId = $(this).attr("href").replace("#", "");
        var index = $(this).data("index");
        
        $("#inputTx").val(txId);
        $("#inputIndex").val(index);
        
        return false;
      });
      
      $("body").on("click", "[data-provide='view-tx-info']", function(){
        var txId = $(this).attr("href").replace("#", "");
        var transaction = _.findWhere(window.loadedTransactions, {hash: txId});
        
        $("[data-provide='tx-info']").html( JSON.stringify(transaction, null, '  ') ).removeClass("hidden");
        
        return false;      
      });
      
      $("body").on("click", "[data-provide='refresh-balance']", function(){
        var address = $("#sendAddress").val();        
        $("[data-provide='balance-container'] [data-provide='loader']").removeClass("hidden");
        
        Chain.getAddressInfo( address ).done(function(data){                
          var template = _.template( $("#balanceTemplate").text() );     
          window.loadedTransactions = data.transactions;            
          $("[data-provide='balance-container']").html( template(data) );                    
        });
                        
        return false;
      });
      
      $("[data-provide='import']").click(function(){
        
        var privKey = $("#privKey").val();
        
        if( privKey.length == 0 ){
          alert("You need to specify a private key value.");
          return false;  
        }
        
        try {
          var key = bitcoin.ECKey.fromWIF(privKey);
          var address = key.pub.getAddress().toString();        
          $("#sendAddress").val( address );
        }catch(e){
          alert("Error with key: " + e.message);
        }
        
        return false;
      });
      
      $("[data-provide='generate-keys']").submit(function(){
      
        if( $("#privKey").val().length || $("#sendAddress").val().length ){
          alert("Key values not blank. Won't generate.");
          return false;
        }
      
        var key = bitcoin.ECKey.makeRandom();
        var privKey = key.toWIF(CURRENT_NETWORK);
        var address = key.pub.getAddress(CURRENT_NETWORK).toString();
        
        $("#privKey").val( privKey );
        $("#sendAddress").val( address );
        
        return false;
      });
      
    });
  </script>

  </body>
</html>
