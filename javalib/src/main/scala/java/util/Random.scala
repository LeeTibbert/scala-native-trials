package java.util

import scala.annotation.tailrec

/** Ported from Apache Harmony and described by Donald E. Knuth in The Art of
 *  Computer Programming, Volume 2: Seminumerical Algorithms, section 3.2.1.
 */
class Random(seed_in: Long) extends AnyRef with java.io.Serializable {

  private var seed: Long = calcSeed(seed_in)

  // see nextGaussian()
  private var nextNextGaussian: Double = _
  private var haveNextNextGaussian: Boolean = false

  private def calcSeed(seed_in: Long): Long =
    (seed_in ^ 0x5deece66dL) & ((1L << 48) - 1)

  def this() = {
    this(0) // ensure hashCode is set for this object
    seed = calcSeed(System.currentTimeMillis() + hashCode)
  }

  def setSeed(seed_in: Long): Unit = {
    seed = calcSeed(seed_in)
    haveNextNextGaussian = false
  }

  protected def next(bits: Int): Int = {
    seed = (seed * 0x5deece66dL + 0xbL) & ((1L << 48) - 1)
    (seed >>> (48 - bits)).toInt
  }

  def nextDouble(): Double =
    ((next(26).toLong << 27) + next(27)) / (1L << 53).toDouble

  def nextBoolean(): Boolean = next(1) != 0

  def nextInt(): Int = next(32)

  def nextInt(n: Int): Int = {
    if (n <= 0)
      throw new IllegalArgumentException("n must be positive")

    if ((n & -n) == n) // i.e., n is a power of 2
      ((n * next(31).toLong) >> 31).toInt
    else {
      @tailrec
      def loop(): Int = {
        val bits = next(31)
        val value = bits % n
        if (bits - value + (n - 1) < 0) loop()
        else value
      }

      loop()
    }
  }

  def nextLong(): Long = (next(32).toLong << 32) + next(32)

  def nextFloat(): Float = next(24) / (1 << 24).toFloat

  def nextBytes(bytes: Array[Byte]): Unit = {
    var i = 0
    while (i < bytes.length) {
      var rnd = nextInt()
      var n = Math.min(bytes.length - i, 4)
      while (n > 0) {
        bytes(i) = rnd.toByte
        rnd >>= 8
        n -= 1
        i += 1
      }
    }
  }

  def nextGaussian(): Double = {
    /* The Box-Muller algorithm produces two random numbers at once. We save
     * the second one in `nextNextGaussian` to be used by the next call to
     * nextGaussian().
     *
     * See http://www.protonfish.com/jslib/boxmuller.shtml
     */
    if (haveNextNextGaussian) {
      haveNextNextGaussian = false
      return nextNextGaussian
    }

    var x, y, rds: Double = 0

    /* Get two random numbers from -1 to 1.
     * If the radius is zero or greater than 1, throw them out and pick two new
     * ones.
     * Rejection sampling throws away about 20% of the pairs.
     */
    while ({
      x = nextDouble() * 2 - 1
      y = nextDouble() * 2 - 1
      rds = x * x + y * y
      rds == 0 || rds > 1
    }) ()

    val c = Math.sqrt(-2 * Math.log(rds) / rds)

    // Save y*c for next time
    nextNextGaussian = y * c
    haveNextNextGaussian = true

    // And return x*c
    x * c
  }
}
