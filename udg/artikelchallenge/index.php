<?php
/**
 * One-File application to import, edit, export, visualize and navigate through CSV Data
 * 
 * This application uses Bootstrap3, X-Editable, JQuery 1.9.2, CanvasJS and standard
 * PHP 7.2 without any additional(server side) dependencies to import csv files,
 * visualize them with CanvasJS in charts, edit and save them as csv back on disk.
 * 
 * 
 * @package	UDG_VisualEdit
 * @author	Leon Giesenkämper <leon@darksider3.de>
 * @version	0.4
 * @see	For a description for the challenge please refer to the delivered PDF file(german)
 * @license	GPLv3-only(NOT above)
 */

/* @TODO: Following
 * 
 * Suche(AJAX, in POST am besten! Nicht Volltext, nur einzelne Wörter)
 * 
 * Rate-Limiting
*/

/**
 * Configuration-Variable for the application
 *  
 * @var
 * @global
 * 
 */
$CONFIG = [];
$CONFIG["defaultFile"]["name"] = "./Artikel.csv";
$CONFIG["serial"]["bin"] = "./data_series.bin";
$CONFIG["serial"]["forced_file"] = "./.forced_reload_flag";


//######### CSV ASSIST FUNCTIONS #########
/**
 * Converts the given file to an PHP-Array and returns it in the format `array[index][string(headingName)]`
 * 
 * 
 * @param	string	$filename Filename for CSV-Array to convert.
 * @return	array	The CSV converted to an array.
 * 
 * @example	print_r(csv_to_array("test.csv")[0]["id"]);
 * @example	print_r(csv_to_array("test.csv")[0]["heading123"]);
 * @example	print_r(csv_to_array("test.csv")[0]["Polka"]);
 */
function csv_to_array(string $filename)
{
  $data = $header = array();
  $i = 0;
  $file = fopen($filename, 'r');
  while (($line = fgetcsv($file, 0, ";")) !== false)
  {
    if ($i == 0)
    {
      $header = $line;
    }
    else
    {
      $data[] = $line;
    }
    $i++;
  }
  fclose($file);
  foreach ($data as $key => $_value)
  {
    $new_item = array();
    foreach ($_value as $key => $value)
    {
      $new_item[$header[$key]] = $value;
    }
    $_data[] = $new_item;
  }
  return $_data;
}

/**
 * Converts any given array with format [$integer-index][] to a CSV-string(; delimiter)
 * 
 * 
 * @param	array	&$array An array to create the csv-string from
 * @return	string	The string to work with, CSV formatted(with ; as delimiter)
 * 
 * @example file_put_contents("test.csv", array2csv($dataArray));
 */
function array2csv(array & $array)
{
  if (count($array) == 0)
  {
    return null;
  }
  ob_start();
  $df = fopen("php://output", 'w');
  fputcsv($df, array_keys(reset($array)) , ';');
  foreach ($array as $row)
  {
    fputcsv($df, $row, ';');
  }
  fclose($df);
  $ret = ob_get_clean();
  ob_end_clean();
  return $ret;
}

/**
 * Helper function to write (CSV) to a specific file(REPLACING content!)
 * 
 * 
 * @todo Check for Success, raise exception if not
 * @param	string	$filename	Filename to write data to
 * @param	string	$csv	String to write into said file
 * @return	none
 * 
 * @example	write_csv("test.csv", array2csv($dataArray));
 */
function write_csv($filename, $csv)
{
  file_put_contents($filename, $csv);
}

/**
 * Helper function to create an (empty) flag-file used to trigger rebuilding of the serialization-data
 * 
 * 
 * @todo Check for success, raise exception if not
 * @param	string	$filename	Filename to create
 * @return	none
 * 
 * @example	createForceFile("./.flag-file");
 */
function createForceFile($filename)
{
  touch($filename);
}

/**
 * Helper function to delete the flag-file used to trigger re-serialization
 * 
 * 
 * @todo Check for success, raise exception if not
 * @param	string	$filename	Filename to delete
 * @return	none
 * 
 * @example	removeForceFile("./.flag-file");
 */
function removeForceFile($filename)
{
  unlink($filename);
}


//######### ARRAY HELPERS(STATISTICAL AND SO ON) #########
/**
 * Returns array-keys found at [(int)$index][HERE].
 * 
 * 
 * @param	array	$arr	Array to get the associative names from
 * @return	array	Number indexed array consisting just of the names
 * 
 * @example	print_r(get_array_keys($meData);
 */
function get_array_keys($arr)
{
  $returner = [];
  foreach ($arr as $keys => $values) array_push($returner, $keys);
  return $returner;
}

/**
 * Helper function to count values at a associative array at [index][HERE].
 * Also eliminates empty values('') and replaces it with a set default value
 * 
 * 
 * @param	array	$datapoints	Array to count
 * @param	string	$KeyToCount	Said specific Key in the second dimension to count	
 * @param	string	$defaultsTo	Default name for empty associative names('')
 * 
 * @return	array	Return an array of the format `['valueCounted'] = $counter`
 * 
 * @example print_r(NestedKeyCounter([0]["Mua"]=123, "Mua"); // => '123' => 1
 */
function NestedKeyCounter($datapoints, $KeyToCount, $defaultsTo = 'unisex')
{
  $ret = [];
  $CountedKeys[$defaultsTo] = 0;
  foreach ($datapoints as $arr)
  {
    foreach ($arr as $key => $value)
    {
      if ($key == $KeyToCount)
      {
        if (array_key_exists($value, $CountedKeys))
        {
          if ($value == '') $CountedKeys[$defaultsTo]++;
          else $CountedKeys[$value]++;
        }
        else
        {
          $CountedKeys[$value] = 0;
        }
      }
    }
  }
  unset($CountedKeys['']);
  return $CountedKeys;
}

/**
 * Helper function to write (CSV) to a specific file(REPLACING content!)
 * 
 * @param	string	$filename	Filename to write data to
 * @param	string	$csv	String to write into said file
 * @return	none
 * 
 * @example	write_csv("test.csv", array2csv($dataArray))
 */
function pagesCount($arr, $itemsPerPage)
{
  return count($arr) / $itemsPerPage;
}


//######### STRING HELPERS #########
/**
 * Helper function to write (CSV) to a specific file(REPLACING content!)
 * 
 * @param	string	$filename	Filename to write data to
 * @param	string	$csv	String to write into said file
 * @return	none
 * 
 * @example	write_csv("test.csv", array2csv($dataArray))
 */
function escapestr($str)
{
  return htmlspecialchars($str, ENT_QUOTES, 'UTF-8');
}


//######### $_GET-FLAGS PROCESSING #########
if (isset($_GET["ipp"]) && $_GET["ipp"] != NULL)
{
  $CONFIG["itemsPerPage"] = (int)$_GET["ipp"];
}
else
{
  $CONFIG["itemsPerPage"] = 50;
}

if (isset($_GET["file"]) && $_GET["file"] != NULL)
{
  if (file_exist("./" + $_GET["file"])) $fileLoad = "./" + $_GET["file"];
  else $fileLoad = $CONFIG["defaultFile"]["name"];
}
else
{
  $fileLoad = $CONFIG["defaultFile"]["name"];
}

if (!file_exists($fileLoad))
{
  echo "<html><body><h3>ERROR, Couldnt find $fileLoad!</h3></body></html>";
  exit(1);
}

if (isset($_GET["page"]) && $_GET["page"] != NULL)
{
  /**
   * Current used Index to view Elements from
   * 
   * @var
   * @global
   * 
   * @example On Site 3 with 10 Elements per Page, 3*10=>30==30 used as starting point to show the data to the user
   */
  $index = (int)$_GET["page"] * $CONFIG["itemsPerPage"];
}
else
{
  $index = 0;
}


//######### SERIALISATION #########
if (!file_exists($CONFIG["serial"]["bin"]))
{

  /**
   * CSV-Array to operate on. It's 0-index with associative arrays with the values
   * 
   * @var
   * @global
   * 
   * @example $csvdata[0]["heading"] = "value";
   */
  $csvdata = csv_to_array($fileLoad);
  file_put_contents($CONFIG["serial"]["bin"], serialize($csvdata) , LOCK_EX);
}
else
{
  if (file_exists($CONFIG["serial"]["forced_file"]))
  {
    removeForceFile($CONFIG["serial"]["forced_file"]);
    $csvdata = csv_to_array($fileLoad);
    file_put_contents($CONFIG["serial"]["bin"], serialize($csvdata) , LOCK_EX);
  }
  $csvdata = unserialize(file_get_contents($CONFIG["serial"]["bin"]));
}

//#########  POST-API #########
if (isset($_POST["name"]) && isset($_POST["value"]) && isset($_POST["pk"]))
{
  if (array_key_exists($_POST["name"], $csvdata[$_POST["pk"]]))
  {
    $csvdata[escapestr($_POST["pk"]) ][escapestr($_POST["name"]) ] = escapestr($_POST["value"]);
    write_csv($CONFIG["defaultFile"]["name"], array2csv($csvdata));
    createForceFile($CONFIG["serial"]["forced_file"]);
    echo json_encode(array(
      $csvdata[$_POST["pk"]][$_POST["name"]]
    ));
  }
  else
  {
    echo json_encode(array(
      False
    ));
  }
  exit(0);
}


//######### CHUNKS #########

/**
 * Chunk to show based on the `$index` and Items per Page
 *  
 * @var
 * @global
 * 
 */
$chunk = array_slice($csvdata, $index, $CONFIG["itemsPerPage"]);

/**
 * Associative Array Key names. Used for table headings in the UI
 *  
 * @var
 * @global
 * 
 */
$keys = get_array_keys($csvdata[0]);


//######### STATISTICAL #########

/**
 * Actual link to the current site, needed for things like link-building for this page(FAQ for example)
 * 
 * @var
 * @global
 * 
 */
$ActualLink = "//$_SERVER[HTTP_HOST]" . escapestr($_SERVER["REQUEST_URI"]);

/**
 * Counted array items to do basic math on the data already gathered(csv-data)
 * 
 * @var
 * @global
 * 
 */
$arrayEntries = count($csvdata);

/**
 * Insignificant percentage filter. Filters out overly small categories with an representation unter 1%
 * 
 * @var
 * @global
 * 
 */
$insignificantInStats = $arrayEntries * 0.01;

/**
 * Save calculated data points to this Array for use later on the pie-chart
 * 
 * @var
 * @global
 * 
 */
$dataPointArray = [];
foreach (NestedKeyCounter($csvdata, "Produktart", "unbekannt") as $Prodname => $counter)
{
  if ($counter < $insignificantInStats) continue;

  $insertArray = array(
    "label" => $Prodname,
    "y" => round(($counter / $arrayEntries) * 100, 2)
  );

  array_push($dataPointArray, $insertArray);

}

//######### HTML BOILERPLATE #########
?>
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<title>UDG Editable Table</title>
<!-- jquery -->
<script src="http://code.jquery.com/jquery-1.9.1.min.js"></script>
<!-- x-editable -->
<link href="css/bootstrap.min.css" rel="stylesheet">
<link href="css/bootstrap-editable.css" rel="stylesheet"/>
<script src="js/bootstrap.min.js"></script>
<script src="js/bootstrap-editable.min.js"></script>
<script src="https://canvasjs.com/assets/script/canvasjs.min.js"></script>
<script>
window.onload = function() {

var chart = new CanvasJS.Chart("chartContainer", {
	animationEnabled: true,
	title: {
		text: "Prozentuale Verteilung der Produktarten"
	},
	data: [{
		type: "pie",
		startAngle: 180,
		yValueFormatString: "##0.00\"%\"",
		indexLabel: "{label} {y}",
		dataPoints: <?php echo json_encode($dataPointArray, JSON_NUMERIC_CHECK); ?>
		
	}]
});
chart.render();

}</script>
<body style="padding: 100px 100px">
<header>
<nav class="navbar navbar-inverse" role="navigation">
  <div id="bs-example-navbar-collapse-1">
    <ul class="nav navbar-nav">
      <li class="active"><a href="#">Some</a></li>
      <li><a href="#">Link</a></li>
    </ul>
  </div>
</nav>
</header>
<div id="chartContainer" style="height: 370px; width: 100%"></div> 

<div class="table-responsive">
<table id='user' class='table-striped table-hover table-bordered '>
<thead>
<tr>
<?php

//######### TABLE #########
foreach ($keys as $useless => $headings)
{
  echo "\t<th>";
  echo $headings;
  echo "</th>\n";
}

echo "</tr>\n</thead>\n";

foreach ($chunk as $arr)
{
  echo "<tr>";
  foreach ($arr as $key => $value)
  {
    echo "<td>";
    if ($key == "Beschreibung" || $key == "Materialangaben")
    {
      echo "<textarea class='editable'  placeholder='Beschreibung fehlt' data-name='$key' data-type='textarea' data-pk='$index' data-url='index.php'>$value</textarea>";
    }
    else
    {
      echo "<a href='#' data-name='$key' data-type='text' data-pk='$index' data-url='index.php'>$value</a>";
    }
    echo "</td>";
  }
  echo "</tr>\n";
  $index++;

}
echo "</table></div>"
?>
<section>
<p>

<div class='nav-stacked'>Seiten: 
<ul class='pagination pagination-sm'>
<?php

//######### SIDE COUNTER AND NAVBAR #########
for ($i = 0;$i <= pagesCount($csvdata, $CONFIG['itemsPerPage']);$i++)
{
  if (isset($_GET["page"]) && $i == $_GET["page"])
  {
    echo "<li class='disabled'><a href='?page=$i'>$i</a></li>";
    continue;
  }
  echo "<li><a href='?page=$i'>$i</a></li>\n";
}
?>
</ul>
</p>
</div> 
</section>

<script>   
$(function(){
	$('#user a').editable({
		mode: "inline",
		url: 'index.php',
		emptytext: "Leer"
	});
	$('#user textarea').editable({
		mode: "inline",
		type: "textarea",
		placeholder: "Beschreibung fehlt",
		emptytext: "Leer",
		url: 'index.php',
	});
});
</script>
</body>
</html>
