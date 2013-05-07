<?php
include 'funcs.php';
cookieCheck();
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html>
<head>
  <title>FLAN | search results</title>
<link rel="stylesheet" type="text/css" href="css/html.css" media="screen, projection, tv " />
  <link rel="stylesheet" type="text/css" href="css/layout.css" media="screen, projection, tv" />
  <link rel="icon" href="images/favicon.ico" type="image/x-icon" />
</head>
<body>
<div id="main">
<div id="header">
  <?php mainHead("advanced search");
?>
</div>
<div id="content">
<div class="basic">
<?php
  /*
   * Create short variable name for $searchtitle in order to determine the type of search.
   * Once the search type is determined use the appropriate function to define a query
   * and then call the askQuery() function to process and export the query.
   */
 
  //create $query variable
  $query;
if(isset($_GET['te']) && isset($_GET['ty'])) {
  $query = linkQuery();
  $_POST['writetofile']=false;
    // for returning visitors with a current query
    $cols = $_SESSION['cols'];
    $writetofile = $_SESSION['writetofile'];
} else {
  $searchtitle = $_POST['searchtitle'];
  switch($searchtitle) {
  case "advanced":
    $query = advancedQuery();
    break;
  case "all":
    $query = allApogeeQuery();
    $writetofile=false;
    break;
  case "basic":
    $query = basicQuery();
    $writetofile=false;
    
    break;
  case "open":
    //In the case of open query we simply define $query to be the input
    //and force it to print to a file
    $query = stripslashes ($_POST['openquery']);
    $_POST['writetofile']=true;
    
    break;   
  default:
    // for returning visitors with a current query
    $query = $_SESSION['currQuery'];
    $cols = $_SESSION['cols'];
    $writetofile = $_SESSION['writetofile'];
    break;
  }
}


if($_POST['new'] == 1) {
  // unset old session variables
  unset($_SESSION['currQuery'],$_SESSION['writetofile'],$_SESSION['cols']);
  
  // save the query to the current query session variable
  $_SESSION['currQuery'] = $query;

  //save query options to the current query session variable
  $_SESSION['writetofile'] = $_POST['writetofile'];
  $writetofile = $_POST['writetofile'];
  
  $cols["a"] = $_POST['acol'];//current apogee id
  $cols["b"] = $_POST['bcol'];//verified apogee time
  $cols["c"] = $_POST['ccol'];//alg apogee time
  $cols["d"] = $_POST['dcol'];//diff btwn ver and alg times
  $cols["e"] = $_POST['ecol'];//ver let
  $cols["f"] = $_POST['fcol'];//alg let
  $cols["g"] = $_POST['gcol'];//duration from prev to next apogee
  $cols["h"] = $_POST['hcol'];//MAD
  $cols["i"] = $_POST['icol'];//word
  $cols["j"] = $_POST['jcol'];//position in word
  $cols["k"] = $_POST['kcol'];//word type
  $cols["l"] = $_POST['lcol'];//word length
  //$cols["m"] = $_POST['mcol'];//word length category
  //$cols["n"] = $_POST['ncol'];
  $cols["o"] = $_POST['ocol'];//button type
  $cols["p"] = $_POST['pcol'];//pp apogee time
  $cols["q"] = $_POST['qcol'];//pp apogee let
  $cols["r"] = $_POST['rcol'];//p apogee time
  $cols["s"] = $_POST['scol'];//p apogee let
  $cols["t"] = $_POST['tcol'];//prev curr diff
  $cols["u"] = $_POST['ucol'];//f apogee time
  $cols["v"] = $_POST['vcol'];//f apogee let
  $cols["w"] = $_POST['wcol'];//next curr diff
  $cols["x"] = $_POST['xcol'];//ff apogee time
  $cols["y"] = $_POST['ycol'];//ff apogee let
  $cols["z"] = $_POST['zcol'];//signer
  $cols["aa"] = $_POST['aacol'];//speed
  $cols["bb"] = $_POST['bbcol'];//segment
  $_SESSION['cols'] = $cols;
}  

askQuery($query,$writetofile,$cols);


?>
</div>
</div>
</div>

  <?php mainFoot();?>


</body>
</html>
<?php
exit;


function allApogeeQuery() {
//this query returns all of the apogees in the database without any filtering
    $query = "SELECT apogees.id apogeeID,
    apogees.verApogee, 
    verlet.letter verLetter, 
    apogees.algApogee, 
    alglet.letter algLetter,
    foll1.verApogee-prev1.verApogee span, 
    apogees.ppapogee,
    prev2.verApogee pp_apogee,
    prev2let.letter pp_letter, 
    apogees.papogee, 
    prev1.verApogee p_apogee,
    prev1let.letter p_letter, 
    apogees.fapogee, 
    foll1.verApogee f_apogee,
    foll1let.letter f_letter, 
    apogees.ffapogee, 
    foll2.verApogee ff_apogee,
    foll2let.letter ff_letter, 
    apogees.mad, 
    apogees.position,
    apogees.wordInst,
    words.word,
    wordTypes.type wordType,
    CHAR_LENGTH(words.word) wd_length, 
    buttonTypes.type, 
    signers.signer, 
    speeds.speed, 
    sessions.segment,
    apogees.verApogee - apogees.algApogee apogee_diff,
    apogees.verApogee - prev1.verApogee prev_curr_diff,
    foll1.verApogee - apogees.verApogee curr_next_diff,
	apogees.image
    
    

    FROM apogees 
    INNER JOIN wordInstances
          ON apogees.wordInst = wordInstances.id
    INNER JOIN words
          ON wordInstances.word = words.id
    INNER JOIN wordTypes
          ON words.wordType = wordTypes.id
    INNER JOIN sessions
          ON wordInstances.session = sessions.id
    INNER JOIN signers
          ON sessions.signer = signers.id
    INNER JOIN speeds
          ON sessions.speed = speeds.id
    INNER JOIN buttonTypes
          ON wordInstances.buttonType = buttonTypes.id
    INNER JOIN letters verlet
          ON apogees.verLetter = verlet.id
    INNER JOIN letters alglet
          ON apogees.algLetter = alglet.id

    LEFT OUTER JOIN apogees prev2
          ON apogees.ppapogee = prev2.id
    LEFT OUTER JOIN letters prev2let
          ON prev2.verLetter = prev2let.id

    LEFT OUTER JOIN apogees prev1
          ON apogees.papogee = prev1.id
    LEFT OUTER JOIN letters prev1let
          ON prev1.verLetter = prev1let.id

    LEFT OUTER JOIN apogees foll1
          ON apogees.fapogee = foll1.id
    LEFT OUTER JOIN letters foll1let
          ON foll1.verLetter = foll1let.id

    LEFT OUTER JOIN apogees foll2
          ON apogees.ffapogee = foll2.id
    LEFT OUTER JOIN letters foll2let
          ON foll2.verLetter = foll2let.id
    ORDER BY apogees.verApogee";
    return $query;
  } 

function basicQuery() {
    //create short variable names
    $searchtype = $_POST['searchtype'];
    $searchterm = trim($_POST['searchterm']);

    //check that both variables were assigned
    if(!$searchtype || !$searchterm) {
     echo 'You did not enter the required search parameters. Please go back and try again.';
     exit; 
    }
  
    $correcttype = false;
    //Could get rid of $correcttype but switch table is also being used to define the search term
    $search;
    switch($searchtype) {
      case "apogees.id":
        $correcttype = is_numeric($searchterm);
        $search = "$searchtype =\"$searchterm\"";      
        break;
      case "verlet.letter":
        $correcttype = true;
        $search = "$searchtype LIKE \"%$searchterm%\"";
        break;
      case "words.word":
        //Even though there may be errors in a word, the intended word should be only letters
        $correcttype = ctype_alpha($searchterm);
        $search = " $searchtype = '$searchterm'";
        break;
      case "sequence":
        $correcttype = ctype_alpha($searchterm);
        $searchtype = "words.word";
        $search = "$searchtype LIKE \"%$searchterm%\"";
    }
  
    if(!$correcttype) {
     echo "<font size='4'><p>Search term ";
     echo "<strong>$searchterm</strong>";
     echo " does not match search type ";
     echo "<strong>$searchtype</strong>.<br />";
     echo "Please try a different search.</p></font>";
     exit;
    }
  
    if(!get_magic_quotes_gpc()) {
     $searchtype = addslashes($searchtype);
     $searchterm = addslashes($searchterm); 
    }   
    $query = "SELECT apogees.id apogeeID,
              apogees.verApogee, 
              verlet.letter verLetter, 
              apogees.algApogee, 
              alglet.letter algLetter,
              foll1.verApogee-prev1.verApogee span, 
              apogees.ppapogee, 
              prev2.verApogee pp_apogee,
              prev2let.letter pp_letter, 
              apogees.papogee, 
              prev1.verApogee p_apogee,
              prev1let.letter p_letter, 
              apogees.fapogee, 
              foll1.verApogee f_apogee,
              foll1let.letter f_letter, 
              apogees.ffapogee, 
              foll2.verApogee ff_apogee,
              foll2let.letter ff_letter, 
              apogees.mad, 
              apogees.position, 
              apogees.wordInst,
              words.word,
              wordTypes.type wordType,
              CHAR_LENGTH(words.word) wd_length, 
              buttonTypes.type, 
              signers.signer, 
              speeds.speed, 
              sessions.segment,
              apogees.wordInst,
              apogees.verApogee - apogees.algApogee apogee_diff,
              apogees.verApogee - prev1.verApogee prev_curr_diff,
              foll1.verApogee - apogees.verApogee curr_next_diff,
			 apogees.image
              
              FROM apogees 

              INNER JOIN wordInstances
                    ON apogees.wordInst = wordInstances.id
              INNER JOIN words
                    ON wordInstances.word = words.id
              INNER JOIN wordTypes
                    ON words.wordType = wordTypes.id
              INNER JOIN sessions
                    ON wordInstances.session = sessions.id
              INNER JOIN signers
                    ON sessions.signer = signers.id
              INNER JOIN speeds
                    ON sessions.speed = speeds.id
              INNER JOIN buttonTypes
                    ON wordInstances.buttonType = buttonTypes.id
              INNER JOIN letters verlet
                    ON apogees.verLetter = verlet.id
              INNER JOIN letters alglet
                    ON apogees.algLetter = alglet.id

              LEFT OUTER JOIN apogees prev2
                    ON apogees.ppapogee = prev2.id
              LEFT OUTER JOIN letters prev2let
                    ON prev2.verLetter = prev2let.id

              LEFT OUTER JOIN apogees prev1
                    ON apogees.papogee = prev1.id
              LEFT OUTER JOIN letters prev1let
                    ON prev1.verLetter = prev1let.id

              LEFT OUTER JOIN apogees foll1
                    ON apogees.fapogee = foll1.id
              LEFT OUTER JOIN letters foll1let
                    ON foll1.verLetter = foll1let.id

              LEFT OUTER JOIN apogees foll2
                    ON apogees.ffapogee = foll2.id
              LEFT OUTER JOIN letters foll2let
                    ON foll2.verLetter = foll2let.id
              WHERE $search
              ORDER BY signers.signer, speeds.speed, sessions.segment, 
                       words.word, wordInstances.id, apogees.position";
        $_POST['acol'] = true;//current apogee id
    $_POST['bcol'] = true;//verified apogee time
    $_POST['ccol'] = true;//alg apogee time
    $_POST['dcol'] = true;//diff btwn ver and alg times
    $_POST['ecol'] = true;//ver let
    $_POST['fcol'] = true;//alg let
    $_POST['gcol'] = true;//duration from prev to next apogee
    $_POST['hcol'] = true;//MAD
    $_POST['icol'] = true;//word
    $_POST['jcol'] = true;//position in word
    $_POST['kcol'] = true;//word type
    $_POST['lcol'] = true;//word length
    //THERE IS NO m- OR n- col
    $_POST['ocol'] = true;//button type
    $_POST['pcol'] = true;//pp apogee time
    $_POST['qcol'] = true;//pp apogee let
    $_POST['rcol'] = true;//p apogee time
    $_POST['scol'] = true;//p apogee let
    $_POST['tcol'] = true;//prev curr diff
    $_POST['ucol'] = true;//f apogee time
    $_POST['vcol'] = true;//f apogee let
    $_POST['wcol'] = true;//next curr diff
    $_POST['xcol'] = true;//ff apogee time
    $_POST['ycol'] = true;//ff apogee let
    $_POST['zcol'] = true;//signer
    $_POST['aacol'] = true;//speed
    $_POST['bbcol'] = true;//segment

    return $query; 
    
  }
  
  function advancedQuery(){
  //create short variable names
  //these variables are trimmed to eliminate any extra whitespace
  $apogeesearch = trim($_POST['apogeesearch']);
  $lettersearch = trim($_POST['lettersearch']);
  $wordsearch = trim($_POST['wordsearch']);
  $papogeesearch = trim($_POST['papogeesearch']);
  $ppapogeesearch = trim($_POST['ppapogeesearch']);
  $fapogeesearch = trim($_POST['fapogeesearch']);
  $ffapogeesearch = trim($_POST['ffapogeesearch']);
  $specialsearchterm = trim($_POST['specialsearchterm']);
  //since the following did not allow for user input, trimming is not necessary
  $wordTypesearch = $_POST['wordTypesearch'];
  $buttonsearch = $_POST['buttonsearch'];
  $signersearch = $_POST['signersearch'];
  $speedsearch = $_POST['speedsearch'];
  $sessionsearch = $_POST['sessionsearch'];
  $orderingterm = $_POST['orderingterm'];
  $specialsearch = $_POST['specialsearch'];
  $startapogee = $_POST['startapogee'];
  $endapogee = $_POST['endapogee'];
  $starttime = $_POST['starttime'];
  $endtime = $_POST['endtime'];
  $minlen = $_POST['minlen'];
  $maxlen = $_POST['maxlen'];
  $seqletters = $_POST['seqletters'];


  //check that at least one variable was assigned
  //$specialsearch will be handled separately
  if(!$apogeesearch && !$lettersearch && !$wordsearch && !$papogeesearch && !$ppapogeesearch
      && !$fapogeesearch && !$ffapogeesearch && !$signersearch && !$buttonsearch && !$wordTypesearch
      && !$speedsearch && !$specialsearch && !$startapogee && !$endapogee && !$starttime && !$endtime
      && !$minlen && !$maxlen) {
    $query = allApogeeQuery();
    return $query;
  }
  //check that the user did not enter a start time that occurs after the end time
  if($starttime && $endtime) {
   if($endtime < $starttime) {
      echo 'Error: start time '.$starttime.' is greater than end time '.$endtime.'.';
      echo '<br />Please go back and try again.';
      exit;
   }
  }
  //check that the end apogee does not occur before the start apogee
  if($startapogee && $endapogee) {
   if($endapogee < $startapogee) {
      echo 'Error: start apogee '.$startapogee.' is after end apogee '.$endapogee.'.';
      echo '<br />Please go back and try again.';
      exit;
   }
  }
  if($minlen && $maxlen) {
   if($maxlen < $minlen) {
      echo 'Error: Minimum length '.$minlen.' is longer than maximum length'.$maxlen.'.';
      echo '<br />Please go back and try again.';
      exit;
   }
  }
  //if we want to search for a sequence of letters rather than the exact word, we need to add
  // '%' around the word
  if(!$seqletters) {
   $wordsearch = '%'.$wordsearch.'%'; 
  }
  
  $wherestmt = "WHERE ";
  //The array consists of the input variable and then the search term for that input
  $wherearray = array($apogeesearch, "apogees.verApogee  LIKE",
                      $lettersearch, "verlet.letter  LIKE",
                      $wordsearch, "words.word  LIKE",
                      $papogeesearch, "prev1let.letter  LIKE",
                      $ppapogeesearch, "prev2let.letter  LIKE",
                      $fapogeesearch, "foll1let.letter  LIKE",
                      $ffapogeesearch, "foll2let.letter  LIKE",
                      $wordTypesearch, "wordTypes.type  LIKE",
                      $buttonsearch, "buttonTypes.type  LIKE",
                      $signersearch, "signers.signer  LIKE",
                      $speedsearch, "speeds.speed  LIKE",
                      $sessionsearch, "sessions.segment  LIKE",
                      $startapogee, "apogees.id >=",
                      $endapogee, "apogees.id <=",
                      $starttime, "apogees.verApogee >=",
                      $endtime, "apogees.verApogee <=",
                      $minlen, "CHAR_LENGTH(words.word) >=",
                      $maxlen, "CHAR_LENGTH(words.word) <="); 
  $arraylen = count($wherearray);
  $firstterm = true;
  //step through the array two entries at a time
  //if variable is NULL then skip it and its search term
  //if variable is NOT NULL add the search term and the variable to the where statement array 
  for($i=0; $i < $arraylen; $i+=2) {
   $currentterm = $wherearray[$i];
   if($currentterm <> "") {
     $toadd = "";
     if(!$firstterm) {
       $toadd .= " AND ";
     }
     $toadd .= $wherearray[$i+1];
     $toadd .= " \"".$currentterm."\"";
     $wherestmt .= $toadd;    
     $firstterm = false;
   }
  }

  //This is where we will deal with the specialsearch
  if($specialsearch <> "") {
    if(!$firstterm) {
     $wherestmt .= " AND ";   
    }
    switch($specialsearch) {
      case "veralgsame":
        $wherestmt .= "verlet.letter LIKE alglet.letter";
        break;
      case "veralgdiff":
        $wherestmt .= "verlet.letter NOT LIKE alglet.letter";
        break;
      case "madgreater":
        if(!$specialsearchterm) {
         echo "This type of search: $specialsearch search requires a search term.";
         echo "<br />Please go back and try again.";
         exit; 
        }
        $wherestmt .= "apogees.mad > $specialsearchterm";
        break;
      case "madless":
        if(!$specialsearchterm) {
         echo "This type of search: $specialsearch search requires a search term.";
         echo "<br />Please go back and try again.";
         exit; 
        }
        $wherestmt .= "apogees.mad < $specialsearchterm";
        break; 
    }
  }
  
  //now we will create the order by statement
  $orderstmt = "ORDER BY ";
  $order = "";
  if($orderingterm) {
    switch($orderingterm){
      case "letter":
        $order = "verlet.letter, words.word, wordInstances.id";
        break;
      case "word":
        $order = "words.word, wordInstances.id, apogees.position";
        break;
      case "position":
        $order = "apogees.position, verlet.letter";
        break;
      case "coderspread":
        $order = "apogees.mad";
        break;
        //ISSUES IMPLEMENTING THE FOLLOWING BECAUSE NOT DEFINED FOR ALL VALUES
//      case "veralgtime":
//        $order = "apogees.verApogee - apogees.algApogee";
//        break;
//      case "nowprevtime":
//        $order = "apogees.verApogee - prev1.verApogee";
//        break;
//      case "nownexttime":
//        $order = "foll1.verApogee - apogees.verApogee";
//        break;
      default:
        $order = "apogees.id";
    }
  } else {
    //I don't think that this actually has to be here, but might as well leave
     $order = "apogees.id";
  }
  $orderstmt .= $order;
  
  
  if(!get_magic_quotes_gpc()) {
   $searchtype = addslashes($searchtype);
   $searchterm = addslashes($searchterm); 
  } 
  //Now put the entire query together witht the wherestatment and the orderby statement 
      $query = "SELECT apogees.id apogeeID,
      apogees.verApogee, 
verlet.letter verLetter, 
apogees.algApogee, 
alglet.letter algLetter,
foll1.verApogee-prev1.verApogee span, 
apogees.ppapogee, 
prev2.verApogee pp_apogee,
prev2let.letter pp_letter, 
apogees.papogee, 
prev1.verApogee p_apogee,
prev1let.letter p_letter, 
apogees.fapogee, 
foll1.verApogee f_apogee,
foll1let.letter f_letter, 
apogees.ffapogee, 
foll2.verApogee ff_apogee,
foll2let.letter ff_letter, 
 apogees.mad, 
apogees.position, 
apogees.wordInst,
words.word,
wordTypes.type wordType,
CHAR_LENGTH(words.word) wd_length, 
buttonTypes.type, 
signers.signer, 
speeds.speed, 
sessions.segment,
apogees.wordInst,
apogees.verApogee - apogees.algApogee apogee_diff,
apogees.verApogee - prev1.verApogee prev_curr_diff,
foll1.verApogee - apogees.verApogee curr_next_diff,
apogees.image

FROM apogees 

INNER JOIN wordInstances
      ON apogees.wordInst = wordInstances.id
INNER JOIN words
      ON wordInstances.word = words.id
INNER JOIN wordTypes
      ON words.wordType = wordTypes.id
INNER JOIN sessions
      ON wordInstances.session = sessions.id
INNER JOIN signers
      ON sessions.signer = signers.id
INNER JOIN speeds
      ON sessions.speed = speeds.id
INNER JOIN buttonTypes
      ON wordInstances.buttonType = buttonTypes.id
INNER JOIN letters verlet
      ON apogees.verLetter = verlet.id
INNER JOIN letters alglet
      ON apogees.algLetter = alglet.id

LEFT OUTER JOIN apogees prev2
      ON apogees.ppapogee = prev2.id
LEFT OUTER JOIN letters prev2let
      ON prev2.verLetter = prev2let.id

LEFT OUTER JOIN apogees prev1
      ON apogees.papogee = prev1.id
LEFT OUTER JOIN letters prev1let
      ON prev1.verLetter = prev1let.id

LEFT OUTER JOIN apogees foll1
      ON apogees.fapogee = foll1.id
LEFT OUTER JOIN letters foll1let
      ON foll1.verLetter = foll1let.id

LEFT OUTER JOIN apogees foll2
      ON apogees.ffapogee = foll2.id
LEFT OUTER JOIN letters foll2let
      ON foll2.verLetter = foll2let.id
$wherestmt
$orderstmt";
return $query;
  }
  
function linkQuery() {
    /*
     * This function takes the GET variables defined in createLink()
     * and uses the information defined there to ask a query for a specific apogee
     * or word instance.
     * This is intended to be used in the apogees results table so that when someone clicks
     * on a apogee name/time and a word, we can go to a page that shows just that specific
     * apogee or word instance.
     */
    $searchterm = $_GET['te'];
    $searchtype = $_GET['ty'];
    //$searchtype will either be a apogee search or a word search in which case we need the word instance
    switch($searchtype) {
      //either w for word or p for apogee
      case 'w':
        //this is for a word
        $searchtype = "apogees.wordInst";
        break;
      case 'p':
        //this is for the current apogee noPqr
        $searchtype = "apogees.verApogee";
        break; 
    }
    //We have to set all of the field values to true
    $_POST['acol'] = true;//current apogee id
    $_POST['bcol'] = true;//verified apogee time
    $_POST['ccol'] = true;//alg apogee time
    $_POST['dcol'] = true;//diff btwn ver and alg times
    $_POST['ecol'] = true;//ver let
    $_POST['fcol'] = true;//alg let
    $_POST['gcol'] = true;//duration from prev to next apogee
    $_POST['hcol'] = true;//MAD
    $_POST['icol'] = true;//word
    $_POST['jcol'] = true;//position in word
    $_POST['kcol'] = true;//word type
    $_POST['lcol'] = true;//word length
    //THERE IS NO m- OR n- col
    $_POST['ocol'] = true;//button type
    $_POST['pcol'] = true;//pp apogee time
    $_POST['qcol'] = true;//pp apogee let
    $_POST['rcol'] = true;//p apogee time
    $_POST['scol'] = true;//p apogee let
    $_POST['tcol'] = true;//prev curr diff
    $_POST['ucol'] = true;//f apogee time
    $_POST['vcol'] = true;//f apogee let
    $_POST['wcol'] = true;//next curr diff
    $_POST['xcol'] = true;//ff apogee time
    $_POST['ycol'] = true;//ff apogee let
    $_POST['zcol'] = true;//signer
    $_POST['aacol'] = true;//speed
    $_POST['bbcol'] = true;//segment

    
    $search = "$searchtype = \"$searchterm\"";
    
    $query = "SELECT apogees.id apogeeID,
              apogees.verApogee, 
              verlet.letter verLetter, 
              apogees.algApogee, 
              alglet.letter algLetter,
              foll1.verApogee-prev1.verApogee span, 
              apogees.ppapogee, 
              prev2.verApogee pp_apogee,
              prev2let.letter pp_letter, 
              apogees.papogee, 
              prev1.verApogee p_apogee,
              prev1let.letter p_letter, 
              apogees.fapogee, 
              foll1.verApogee f_apogee,
              foll1let.letter f_letter, 
              apogees.ffapogee, 
              foll2.verApogee ff_apogee,
              foll2let.letter ff_letter, 
              apogees.mad, 
              apogees.position position,
              apogees.wordInst word_id, 
              images.id imageID,
              words.word word,
              wordTypes.type wordType,
              CHAR_LENGTH(words.word) wd_length, 
              buttonTypes.type, 
              signers.signer, 
              speeds.speed, 
              sessions.segment,
              apogees.wordInst,
              apogees.verApogee - apogees.algApogee apogee_diff,
              apogees.verApogee - prev1.verApogee prev_curr_diff,
              foll1.verApogee - apogees.verApogee curr_next_diff,
			  apogees.image
              
              FROM apogees 

	      RIGHT OUTER JOIN images
                    ON apogees.id = images.apogeeID

              INNER JOIN wordInstances
                    ON apogees.wordInst = wordInstances.id
              INNER JOIN words
                    ON wordInstances.word = words.id
              INNER JOIN wordTypes
                    ON words.wordType = wordTypes.id
              INNER JOIN sessions
                    ON wordInstances.session = sessions.id
              INNER JOIN signers
                    ON sessions.signer = signers.id
              INNER JOIN speeds
                    ON sessions.speed = speeds.id
              INNER JOIN buttonTypes
                    ON wordInstances.buttonType = buttonTypes.id
              INNER JOIN letters verlet
                    ON apogees.verLetter = verlet.id
              INNER JOIN letters alglet
                    ON apogees.algLetter = alglet.id

              LEFT OUTER JOIN apogees prev2
                    ON apogees.ppapogee = prev2.id
              LEFT OUTER JOIN letters prev2let
                    ON prev2.verLetter = prev2let.id

              LEFT OUTER JOIN apogees prev1
                    ON apogees.papogee = prev1.id
              LEFT OUTER JOIN letters prev1let
                    ON prev1.verLetter = prev1let.id

              LEFT OUTER JOIN apogees foll1
                    ON apogees.fapogee = foll1.id
              LEFT OUTER JOIN letters foll1let
                    ON foll1.verLetter = foll1let.id

              LEFT OUTER JOIN apogees foll2
                    ON apogees.ffapogee = foll2.id
              LEFT OUTER JOIN letters foll2let
                    ON foll2.verLetter = foll2let.id
              WHERE $search
              ORDER BY apogees.position";
    //If we are doing a word search then we want it to be sorted by order in word
    //If it is a apogee search there is only one result and order is irrelevant
    return $query;
  }
  
function askQuery($query,$writetofile,$cols) {

    $dbconn = flanConnect();
    $result = $dbconn->query($query);
    if(!$result) {
     echo 'Error: You dastardly demon, you searched for something that is not in the database.<br />';
     echo 'Please go back and try again. <br />';
     echo $query;
     exit; 
    }

// WORD INSTANCE DISPLAY BEGIN

    /* Special case for word instance search - Displays apogee diagram. */
    $searchtype = $_GET['ty'];
    if(($searchtype == "w") && ($_POST['new'] != 1)) {
       echo "<div class=boxCenter><a href='allSearches.php'>back to results</a></div></div><br>";

       /* get query results for multiple uses (Madness! Just copying the results into a new variable causes problems!) */
       $result_img = $dbconn->query($query);
       $result_dur = $dbconn->query($query);
       $word_dur = 0;
       $scale = 200;

       /* print row of apogees with mouse-over drop image */
       $index = 0;
       while ($row_img = $result_img->fetch_assoc()) {
	 $id_temp = $row_img['imageID'];
	 $diff = $row_img['prev_curr_diff'];
	 if($row_img['position'] == 0) {
	   $offset = 54;
	 }
	 else {
	   $offset = ($diff * 100/$scale) - 15;
	 }
	 echo '<span class="dropi" title="" style="margin-left:'.$offset.'px; margin-top:3px; width:15px;">'.$row_img['verLetter'].'
                 <span><img src="view.php?image_id='.$id_temp.'" style="width:500px; height:auto;"><br/>
                 Apogee: '.$row_img['verLetter'].'
                 </span>
                 </span>';
       }


       /* Print apogee positions against ruler */
       /* accumulate word duration, then round up to the nearest multiple of $scale for ruler boundary */
       while ($row_dur = $result_dur->fetch_assoc()) {
	 $diff = $row_dur['prev_curr_diff'];
	 /* Duration will need to be queried later */
	 $duration = 10 * 100/$scale;
	 /* Will need to be adjusted for variable duration apogees */
	 if($row_dur['position'] == 0) {
	   echo '<div class="apog" style="width:'.$duration.'px;margin-left:'.'54'.'px;clear:both;"></div>';
	 }
	 else {
	   $offset = ($diff * 100/$scale) - $duration;
	   $word_dur += $diff;
	   echo '<div class="apog" style="width:'.$duration.'px;margin-left:'.$offset.'px;"></div>';
	 }
	}

       /* Find appropriate end value for the ruler */
       $ruler_end = $word_dur;
       while ($ruler_end % $scale != 0) {
	 $ruler_end++;
       }
       
       /* Print ruler */
       $base = $ruler_end * (100/$scale) + 8;
       $index = $ruler_end/$scale;	  
       echo '<div class="ruler_end" style="clear:both; margin-left:50px;margin-right:1px;"></div>';
       while ($index > 1) {
	 echo '<div class="tick3"></div>';
	 echo '<div class="tick2"></div>';
	 echo '<div class="tick3"></div>';
	 echo '<div class="tick1"></div>';
	 $index--;
       }
       echo '<div class="tick3"></div>';
       echo '<div class="tick2"></div>';
       echo '<div class="tick3"></div>';
       echo '<div class="ruler_end" style="margin-left:24px;"></div>';
       echo '<div class="ruler_base" style="width:'.$base.'px;"></div>';

       /* Print numbers at correct intervals */
       $numbering = $scale;
       echo '<div style="float:left;padding-right:22px;margin-left:50px;position:relative; top:-45px;clear:both;">0</div>';
       while ($numbering <= $ruler_end) {
	 echo '<div class="num" style="margin-left:50px;position:relative; top:-45px;">'.$numbering.'</div>';
	 $numbering += $scale;
       }
    }  
  	
   $num_results = $result->num_rows;
   echo '<br>';
   echo "<p> Number of results found: ".$num_results."</p>";

   if(!$num_results) {
      echo "Error: Query failed to return results.";
      exit; 
    }

// WORD INSTANCE DISPLAY END 

   if($writetofile) {
     srand(time());
     $uniquefilename = false;
     /*
      * Check that the filename generated is not a filename that already exists.
      * 
      */
     while(!$uniquefilename) {
      $val = rand();
      $pathname = "CASL_data";
      if(!is_dir($pathname)) {
        //I don't think that we currently have permission to create a function
       mkdir($pathname);
      }
      $filename = "$pathname/CASL_data_$val.csv";
      if(!file_exists($filename)) {
       $uniquefilename = true; 
      }
     }
    
     if(($fp = fopen($filename, 'w')) == false) {
      echo 'Error opening file';
      exit; 
     }
     
   }
   echo '<fieldset>';
    $openquery = isset($_POST['openquery']);
    $searchterm = $_GET['te'];
    //If this is not an open query add the following headers.
    //No headers are provided for open query.
    if(!$openquery) {
      $headers = "";
      echo '<table align="left" cellspacing="0" cellpadding="5" border="1"><tr>';
    if($cols["a"]) {
      echo '<td align="left"><b>Apogee ID (image)</b></td>';
      $headers .= 'Apogee ID,';
    }
    if($cols["b"]) {
      echo '<td align="left"><b>Verified Apogee (msec)</b></td>';
      $headers .= 'Verified Apogee,';
    }
    if($cols["c"]) {
      echo '<td align="left"><b>Algorithm Apogee (msec)</b></td>';
      $headers .= 'Algorithm Apogee,';
    }
    if($cols["d"]) {
      echo '<td align="left"><b>Apogee Difference (msec)</b></td>';
      $headers .= 'Apogee Diff,';
    }
    if($cols["e"]) {
      echo '<td align="left"><b>verified Letter</b></td>';
      $headers .= 'Verified Letter,';
    }
    if($cols["f"]) {
      echo '<td align="left"><b>Algorithm Letter</b></td>';
      $headers .= 'Algorithm Letter,';
    }
    if($cols["g"]) {
      echo '<td align="left"><b>Duration (msec)</b></td>';
      $headers .= 'span,';
    }
    if($cols["h"]) {
      echo '<td align="left"><b>Coder Spread (MAD) (msec)</b></td>';
      $headers .= 'MAD,';
    }
    if($cols["i"]) {
      echo '<td align="left"><b>Word</b></td>';
      $headers .= 'word,';
    }
    if($cols["j"]) {
      echo '<td align="left"><b>Word Type</b></td>';
      $headers .= 'wordType,';
    }
    if($cols["k"]) {
      echo '<td align="left"><b>Word Length</b></td>';
      $headers .= 'wordlength,';
    }
    if($cols["l"]) {
      echo '<td align="left"><b>Position</b></td>';
      $headers .= 'position,';
    }
    if($cols["o"]) {
      echo '<td align="left"><b>Button Type</b></td>';
      $headers .= 'button type,';
    }
    if($cols["p"]) {
      echo '<td align="left"><b>PrePrev Apogee</b></td>';
      $headers .= 'pp_apogee,';
    }
    if($cols["q"]) {
      echo '<td align="left"><b>PrePrev Apogee Letter</b></td>';
      $headers .= 'pp_let,';
    }
    if($cols["r"]) {
      echo '<td align="left"><b>Prev Apogee</b></td>';
      $headers .= 'p_apogee,';
    }
    if($cols["s"]) {
      echo '<td align="left"><b>Prev Apogee Letter</b></td>';
      $headers .= 'p_let,';
    }
    if($cols["t"]) {
      echo '<td align="left"><b>Diff Prev and Curr Apogee (msec)</b></td>';
      $headers .= 'diff_pre_curr,';
    }
    if($cols["u"]) {
      echo '<td align="left"><b>Next Apogee</b></td>';
      $headers .= 'f_apogee,';
    }
    if($cols["v"]) {
      echo '<td align="left"><b>Next Apogee Letter</b></td>';
      $headers .= 'f_let,';
    }
    if($cols["w"]) {
      echo '<td align="left"><b>Diff Curr and Next Apogee (msec)</b></td>';
      $headers .= 'diff_curr_next,';
    }
    if($cols["x"]) {
      echo '<td align="left"><b>NexNext Apogee</b></td>';
      $headers .= 'ff_apogee,';
    }
    if($cols["y"]) {
      echo '<td align="left"><b>NexNext Apogee Letter</b></td>';
      $headers .= 'ff_let,';
    }
    if($cols["z"]) {
      echo '<td align="left"><b>Signer</b></td>';
      $headers .= 'signer,';
    }
    if($cols["aa"]) {
      echo '<td align="left"><b>Speed</b></td>';
      $headers .= 'speed,';
    }
    if($cols["bb"]) {
      echo '<td align="left"><b>Session</b></td>';
      $headers .= 'session';
    }
    echo '</tr>';  
    
    if($writetofile) {
      $resultarray = array($headers);
    }
    $currentApogees;
    for($i=0; $i<$num_results; $i++) {
      $row = $result->fetch_assoc();
      //format the output for each entry HERE
      //start by opening the table row
      echo '<tr>';
      $resultrow = "";
      $emptyval = 'NULL';
      //Now for the moment go through each column to check it is there and provide 
      //appropriate formatting before outputing info
      if($cols["a"]) {
      $curr_id = createLink("image", $i, $row['apogeeID']);
      echo '<td align="left">'.$curr_id.'</td>';
      $currentApogees[] = $row['apogeeID'];
      $resultrow .=  $row['apogeeID'].','; 
      }
      if($cols["b"]) {      
      $curr_apogee = createLink("p", $row['verApogee'], $row['verApogee']);     
      echo '<td align="left">'.$curr_apogee.'</td>';
      $resultrow .= $row['verApogee'].',';
      }
      if($cols["c"]) {      
      echo '<td align="left">'.$row['algApogee'].'</td>';
      $resultrow .= $row['algApogee'].','; 
      }
      if($cols["d"]) {      
      echo '<td align="left">'.$row['apogee_diff'].'</td>'; 
      $resultrow .= $row['apogee_diff'].',';
      }
      if($cols["e"]) {      
      $curr_let = createLink("p", $row['verApogee'], $row['verLetter']);       
      echo '<td align="left">'.$curr_let.'</td>'; 
      $resultrow .= $row['verLetter'].',';  
      }
      if($cols["f"]) {      
      echo '<td align="left">'.$row['algLetter'].'</td>';
      $resultrow .= $row['algLetter'].',';   
      }
      if($cols["g"]) {      
      echo '<td align="left">'.$row['span'].'</td>';  
      $resultrow .= $row['span'].',';
      }
      if($cols["h"]) {      
      echo '<td align="left">'.$row['mad'].'</td>'; 
      $resultrow .= $row['mad'].',';
      }
      
      /* Word link creatd here */
      if($cols["i"]) {      
      $curr_word = createLink("w", $row['wordInst'], $row['word']);       
      echo '<td align="left">'.$curr_word.'</td>';
      $resultrow .= $row['word'].','; 
      }
      if($cols["j"]) {      
      echo '<td align="left">'.$row['wordType'].'</td>';
      $resultrow .= $row['wordType'].',';  
      }
      if($cols["k"]) {
      echo '<td align="left">'.$row['wd_length'].'</td>'; 
      $resultrow .= $row['wd_length'].','; 
      }
      if($cols["l"]) {      
      echo '<td align="left">'.$row['position'].'</td>';  
      $resultrow .= $row['position'].',';
      }
      //There is currently neither m- or n- col. Don't ask, much pain and suffering involved.
      if($cols["o"]) {      
      echo '<td align="left">'.$row['type'].'</td>';
      $resultrow .= $row['type'].','; 
      }
      if($cols["p"] || $cols["q"]) {
      $ppp = $row['pp_apogee'];
      $pplet = $row['pp_letter'];
  
      if($ppp) {
       $pp_apogeeval = createLink("p", $ppp, $ppp);
       $pp_apogeelet = createLink("p", $ppp, $pplet); 
      } else {
        $pp_apogeeval = $emptyval;
        $pp_apogeelet = $emptyval;
        $ppp = $emptyval;
        $pplet = $emptyval;
      }
        if($cols["p"]) {
          echo '<td align="left">'.$pp_apogeeval.'</td>';
          $resultrow .= $ppp.',';    
        }
        if($cols["q"]) {      
          echo '<td align="left">'.$pp_apogeelet.'</td>'; 
          $resultrow .= $pplet.',';
        }
      }
      if($cols["r"] || $cols["s"] || $cols["t"]) {
      $opp = $row['p_apogee'];
      $plet = $row['p_letter'];
      
      if($opp) {
       $p_apogeeval = createLink("p", $opp, $opp);
       $p_apogeelet = createLink("p", $opp, $plet);
       $p_c_diff = $row['prev_curr_diff']; 
      } else {
        $p_apogeeval = $emptyval;
        $p_apogeelet = $emptyval;
        $opp = $emptyval;
        $plet = $emptyval;
        $p_c_diff = $emptyval;
      }
        if($cols["r"]) {
          echo '<td align="left">'.$p_apogeeval.'</td>'; 
          $resultrow .= $opp.','; 
        }
        if($cols["s"]) {      
          echo '<td align="left">'.$p_apogeelet.'</td>';
          $resultrow .= $plet.',';  
        }
        if($cols["t"]) {      
          echo '<td align="left">'.$p_c_diff.'</td>'; 
          $resultrow .= $p_c_diff.',';
        }
      }
      
      if($cols["u"] || $cols["v"] || $cols["w"]) {
      $ofp = $row['f_apogee'];
      $flet = $row['f_letter'];
      if($ofp) {
       $f_apogeeval = createLink("p", $ofp, $ofp);
       $f_apogeelet = createLink("p", $ofp, $flet);
       $f_c_diff = $row['curr_next_diff']; 
      } else {
        $f_apogeeval = $emptyval;
        $f_apogeelet = $emptyval;
        $ofp = $emptyval;
        $flet = $emptyval;
        $f_c_diff = $emptyval;
      }
        if($cols["u"]) {
          echo '<td align="left">'.$f_apogeeval.'</td>';
          $resultrow .= $ofp.',';   
        }
        if($cols["v"]) {      
          echo '<td align="left">'.$f_apogeelet.'</td>'; 
          $resultrow .= $flet.',';
        }
        if($cols["w"]) {      
          echo '<td align="left">'.$f_c_diff.'</td>'; 
          $resultrow .= $f_c_diff.','; 
        }
      }
      
      if($cols["x"] || $cols["y"]) {
       $ffp = $row['ff_apogee'];
       $fflet = $row['ff_letter']; 
      if($ffp) {
       $ff_apogeeval = createLink("p", $ffp, $ffp);
       $ff_apogeelet = createLink("p", $ffp, $fflet); 
      } else {
        $ff_apogeeval = $emptyval;
        $ff_apogeelet = $emptyval;
        $ffp = $emptyval;
        $fflet = $emptyval;
      }
        if($cols["x"]) {
          echo '<td align="left">'.$ff_apogeeval.'</td>';
          $resultrow .= $ffp.',';  
        }
        if($cols["y"]) {      
          echo '<td align="left">'.$ff_apogeelet.'</td>'; 
          $resultrow .= $fflet.',';
        }
      }
      if($cols["z"]) {      
      echo '<td align="left">'.$row['signer'].'</td>'; 
      $resultrow .= $row['signer'].','; 
      }
      if($cols["aa"]) {      
      echo '<td align="left">'.$row['speed'].'</td>';
      $resultrow .= $row['speed'].',';   
      }
      if($cols["bb"]) {      
      echo '<td align="left">'.$row['segment'].'</td></tr>';
      $resultrow .= $row['segment'];   
      }
      echo '</tr>';
      $_SESSION['currentApogees'] = $currentApogees;
      if($writetofile) {
        $resultarray[] = $resultrow;
      }
    }
    echo '</table></fieldset>';
    } else {
      //If this is an open query, then just flat out dump each row into a table and it's your own fault if
      //you don't know what's what
      
      echo '<table align="left" cellspacing="0" cellpadding="5" border="1">';
      for($i=0; $i<$num_results; $i++) {
        $row = $result->fetch_array();
        $length = count($row);
        //$length returns twice as many cells as needed
        $length = $length/2;
        $stracc.="";
        echo '<tr>';
        for($j=0; $j<$length; $j++) {
         $strelement = $row[$j];
         if(!$strelement) {
          $strelement = "NULL"; 
         }
         echo '<td align="left">'.$strelement.'</td>';
         $stracc .= $strelement;
        }
        echo '</tr>';
        $resultarray[] = $stracc;
        
      }
      echo '</table></fieldset>';
    }
    //write the row to the file regardless of whether it is an open query or not
    //open query always writes to a file, but advanced query may or may not and
    //basic query never does
    if($writetofile) {
      foreach($resultarray as $line) {
       fputcsv($fp, split(',', $line)); 
      }
     fclose($fp);


  //Create a button to download the file
  echo '<form action="download.php" method="post">';
  
  echo '<input type="hidden" name="filename" value= '.$filename.'>';
  echo '<input type="submit" value="Download File">';
  echo '</form>';
 
    }
    $result->free();
    $dbconn->close();
  }

function createLink($searchtype, $searchterm, $text) {
  if($searchtype=='image'){
    $link = "<a href=image.php?num=$searchterm>$text</a>";
  }else{
    //Creates a link that passes the searchtype and searchterm as GET variables.
    $link = "<a href=allSearches.php?ty=$searchtype&te=$searchterm>$text</a>";
  }
    return $link;
  }
?>
