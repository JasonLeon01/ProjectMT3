// Build a distribution-only opaque icon without changing the game's assets.
import Foundation
import CoreGraphics
import ImageIO
import UniformTypeIdentifiers

guard CommandLine.arguments.count == 3,
      let source = CGImageSourceCreateWithURL(URL(fileURLWithPath: CommandLine.arguments[1]) as CFURL, nil)
else { fatalError("Usage: ios-icon.swift <source> <destination.png>") }
let images = (0..<CGImageSourceGetCount(source)).compactMap {
    CGImageSourceCreateImageAtIndex(source, $0, nil)
}
guard let image = images.max(by: { $0.width * $0.height < $1.width * $1.height }),
      let canvas = CGContext(data: nil, width: 1024, height: 1024, bitsPerComponent: 8,
                             bytesPerRow: 0, space: CGColorSpaceCreateDeviceRGB(),
                             bitmapInfo: CGImageAlphaInfo.noneSkipLast.rawValue)
else { fatalError("Unable to decode the project icon") }
canvas.setFillColor(CGColor(red: 0, green: 0, blue: 0, alpha: 1))
canvas.fill(CGRect(x: 0, y: 0, width: 1024, height: 1024))
canvas.interpolationQuality = .high
canvas.draw(image, in: CGRect(x: 0, y: 0, width: 1024, height: 1024))
guard let output = canvas.makeImage(),
      let destination = CGImageDestinationCreateWithURL(
        URL(fileURLWithPath: CommandLine.arguments[2]) as CFURL, UTType.png.identifier as CFString, 1, nil)
else { fatalError("Unable to create the distribution icon") }
CGImageDestinationAddImage(destination, output, nil)
guard CGImageDestinationFinalize(destination) else { fatalError("Unable to write the distribution icon") }
