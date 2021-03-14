object Test {
  def main(args: Array[String]) = {
    val iters = args(0).toInt

// TODO: probably add the start time
    System.err.printf(s"\nString.toDouble() Begin iterations: ${iters}.\n")

    scala.util.Random.setSeed(500L)
    def lat = scala.util.Random.between(-90.0, 90.0)

    for (i <- 0 to iters) {
      val str = lat.toString

      try {
        str.toDouble
      } catch {
        case e =>
          println(s"Failed after $i iterations")
          throw e
      }
    }

// TODO: probably add the start time
    System.err.printf(s"\nString.toDouble() End\n")
  }
}
